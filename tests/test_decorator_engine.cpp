#include "check.h"
#include <brodecor/decor.h>

using namespace brodecor;

int main() {
    DecoratorEngine engine;
    engine.set_client_rect(Rect{50.0f, 50.0f, 800.0f, 600.0f});
    engine.set_title("Hello Engine");

    // 1. Initial configuration
    {
        CHECK_EQ(engine.caption().title, "Hello Engine");
        CHECK_EQ(engine.geometry().client_rect.width, 800.0f);
        CHECK_EQ(engine.geometry().client_rect.height, 600.0f);
        CHECK_TRUE(is_active(engine.state()));
    }

    // 2. State change callback
    {
        bool state_cb_fired = false;
        FrameState cb_state = FrameState::Normal;
        engine.on_state_changed = [&](FrameState s) {
            state_cb_fired = true;
            cb_state = s;
        };

        engine.set_state(FrameState::Maximized);
        CHECK_TRUE(state_cb_fired);
        CHECK_TRUE(is_maximized(cb_state));
        CHECK_TRUE(is_maximized(engine.state()));
        CHECK_NEAR(engine.geometry().corner_radius, 0.0f, 1e-4);

        // Restore to normal active
        engine.set_state(FrameState::Active);
        CHECK_TRUE(is_active(engine.state()));
        CHECK_FALSE(is_maximized(engine.state()));
        CHECK_TRUE(engine.geometry().corner_radius > 0.0f);

        engine.on_state_changed = nullptr;
    }

    // 3. Pointer event routing & click callback
    {
        int clicked_kind = -1;
        engine.on_button_click = [&](ButtonKind k) {
            clicked_kind = static_cast<int>(k);
        };

        REQUIRE(engine.geometry().buttons.size() >= 3u);
        const Rect& close_rect = engine.geometry().buttons.back().rect;
        Vec2 close_center = close_rect.center();

        // Pointer move over Close
        bool moved = engine.on_pointer_move(close_center);
        CHECK_TRUE(moved);
        CHECK_EQ(engine.button_state(ButtonKind::Close), ButtonState::Hover);

        // Tick animation
        engine.tick(0.05f);
        CHECK_TRUE(engine.button_hover_progress(ButtonKind::Close) > 0.0f);

        // Pointer down
        bool down = engine.on_pointer_down(close_center, 1);
        CHECK_TRUE(down);
        CHECK_EQ(engine.button_state(ButtonKind::Close), ButtonState::Pressed);

        // Pointer up -> triggers callback
        bool up = engine.on_pointer_up(close_center, 1);
        CHECK_TRUE(up);
        CHECK_EQ(clicked_kind, static_cast<int>(ButtonKind::Close));
        CHECK_EQ(engine.button_state(ButtonKind::Close), ButtonState::Hover);

        engine.on_pointer_leave();
        CHECK_EQ(engine.button_state(ButtonKind::Close), ButtonState::Normal);

        engine.on_button_click = nullptr;
    }

    // 4. Hit test delegation
    {
        HitTestResult res_client = engine.hit_test(engine.geometry().client_rect.center());
        CHECK_TRUE(res_client.is_client());

        HitTestResult res_caption = engine.hit_test(Vec2{
            engine.geometry().caption_rect.x + 20.0f,
            engine.geometry().caption_rect.center().y
        });
        CHECK_TRUE(res_caption.is_caption());
    }

    // 5. Shadow mesh generation through engine
    {
        // Active floating window generates shadow mesh
        ShadowMesh mesh_active = engine.generate_shadow_mesh(true);
        CHECK_FALSE(mesh_active.empty());
        CHECK_EQ(mesh_active.quad_count(), 9u);

        // Maximized window generates empty shadow
        engine.set_state(FrameState::Maximized);
        ShadowMesh mesh_max = engine.generate_shadow_mesh(true);
        CHECK_TRUE(mesh_max.empty());

        // Fullscreen generates empty shadow
        engine.set_state(FrameState::Fullscreen);
        ShadowMesh mesh_fs = engine.generate_shadow_mesh(true);
        CHECK_TRUE(mesh_fs.empty());
    }

    return ::brotest::finish("test_decorator_engine");
}
