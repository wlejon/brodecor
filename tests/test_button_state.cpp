#include "check.h"
#include <brodecor/decor.h>

using namespace brodecor;

int main() {
    FrameMetrics metrics;
    WindowCaption caption;
    caption.title = "Button State Test";

    Rect client{100.0f, 100.0f, 600.0f, 400.0f};
    FrameGeometry geom = compute_frame_geometry_from_client(
        client, metrics, FrameState::Active, caption
    );

    REQUIRE(geom.buttons.size() == 3u);
    const Rect& close_rect = geom.buttons[2].rect;
    const Rect& min_rect = geom.buttons[0].rect;

    ButtonStateManager mgr(metrics.buttons);

    // 1. Initial states
    {
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Normal);
        CHECK_EQ(mgr.get_state(ButtonKind::Minimize), ButtonState::Normal);
        CHECK_EQ(mgr.get_state(ButtonKind::Maximize), ButtonState::Normal);
        CHECK_NEAR(mgr.get_hover_progress(ButtonKind::Close), 0.0f, 1e-4);
        CHECK_NEAR(mgr.get_press_progress(ButtonKind::Close), 0.0f, 1e-4);
    }

    // 2. Hover transitions
    {
        // Move over Close button
        bool handled = mgr.on_pointer_move(close_rect.center(), geom);
        CHECK_TRUE(handled);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Hover);
        CHECK_EQ(mgr.get_state(ButtonKind::Minimize), ButtonState::Normal);

        // Move away from buttons
        mgr.on_pointer_move(Vec2{150.0f, 150.0f}, geom);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Normal);

        // Move back over Close, then pointer leaves window
        mgr.on_pointer_move(close_rect.center(), geom);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Hover);
        mgr.on_pointer_leave();
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Normal);
    }

    // 3. Press and Click success
    {
        int clicked_kind = -1;
        auto on_click = [&](ButtonKind k) {
            clicked_kind = static_cast<int>(k);
        };

        // Pointer down on Close
        bool down_ok = mgr.on_pointer_down(close_rect.center(), geom, 1);
        CHECK_TRUE(down_ok);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Pressed);

        // Pointer up on Close -> triggers click
        bool up_ok = mgr.on_pointer_up(close_rect.center(), geom, 1, on_click);
        CHECK_TRUE(up_ok);
        CHECK_EQ(clicked_kind, static_cast<int>(ButtonKind::Close));
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Hover);

        mgr.on_pointer_leave();
    }

    // 4. Press and Drag Out (canceling click)
    {
        int clicked_kind = -1;
        auto on_click = [&](ButtonKind k) {
            clicked_kind = static_cast<int>(k);
        };

        // Pointer down on Minimize
        mgr.on_pointer_down(min_rect.center(), geom, 1);
        CHECK_EQ(mgr.get_state(ButtonKind::Minimize), ButtonState::Pressed);

        // Drag out of button
        mgr.on_pointer_move(Vec2{min_rect.x - 50.0f, min_rect.y}, geom);
        CHECK_NE(mgr.get_state(ButtonKind::Minimize), ButtonState::Pressed);

        // Release outside
        mgr.on_pointer_up(Vec2{min_rect.x - 50.0f, min_rect.y}, geom, 1, on_click);
        CHECK_EQ(clicked_kind, -1); // Click was cancelled!
    }

    // 5. Disabled button
    {
        mgr.set_disabled(ButtonKind::Close, true);
        CHECK_TRUE(mgr.is_disabled(ButtonKind::Close));
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Disabled);

        // Hover attempt should be ignored
        mgr.on_pointer_move(close_rect.center(), geom);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Disabled);

        // Press attempt should fail
        bool down = mgr.on_pointer_down(close_rect.center(), geom, 1);
        CHECK_FALSE(down);

        // Re-enable
        mgr.set_disabled(ButtonKind::Close, false);
        CHECK_FALSE(mgr.is_disabled(ButtonKind::Close));
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Normal);
    }

    // 6. Animation ticking
    {
        mgr.reset();
        mgr.on_pointer_move(close_rect.center(), geom);
        CHECK_EQ(mgr.get_state(ButtonKind::Close), ButtonState::Hover);
        CHECK_NEAR(mgr.get_hover_progress(ButtonKind::Close), 0.0f, 1e-4);

        // Tick 0.05 seconds: progress should increase smoothly
        mgr.tick(0.05f);
        float p1 = mgr.get_hover_progress(ButtonKind::Close);
        CHECK_TRUE(p1 > 0.1f && p1 < 1.0f);

        // Several more ticks to reach near 1.0
        for (int i = 0; i < 20; ++i) {
            mgr.tick(0.05f);
        }
        float p_full = mgr.get_hover_progress(ButtonKind::Close);
        CHECK_NEAR(p_full, 1.0f, 0.05);

        // Leave and tick back to 0.0
        mgr.on_pointer_leave();
        for (int i = 0; i < 20; ++i) {
            mgr.tick(0.05f);
        }
        float p_zero = mgr.get_hover_progress(ButtonKind::Close);
        CHECK_NEAR(p_zero, 0.0f, 0.05);
    }

    return ::brotest::finish("test_button_state");
}
