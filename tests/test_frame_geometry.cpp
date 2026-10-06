#include "check.h"
#include <brodecor/decor.h>

using namespace brodecor;

int main() {
    FrameMetrics metrics;
    metrics.caption_height = 30.0f;
    metrics.border_thickness = 4.0f;
    metrics.resize_grip_size = 8.0f;
    metrics.corner_radius = 6.0f;
    metrics.corner_grip_length = 20.0f;
    metrics.button_width = 32.0f;
    metrics.button_height = 22.0f;
    metrics.button_spacing = 4.0f;
    metrics.button_margin_right = 6.0f;
    metrics.button_margin_left = 6.0f;

    WindowCaption caption;
    caption.title = "Test Window";

    // 1. Normal state: from_client and from_frame roundtrip equivalence
    {
        Rect client_in{100.0f, 150.0f, 800.0f, 600.0f};
        FrameGeometry geom1 = compute_frame_geometry_from_client(
            client_in, metrics, FrameState::Active, caption
        );

        // Expected frame rect:
        // left = 100 - 4 = 96
        // top = 150 - (4 + 30) = 116
        // width = 800 + 8 = 808
        // height = 600 + 8 + 30 = 638
        CHECK_NEAR(geom1.frame_rect.x, 96.0f, 1e-4);
        CHECK_NEAR(geom1.frame_rect.y, 116.0f, 1e-4);
        CHECK_NEAR(geom1.frame_rect.width, 808.0f, 1e-4);
        CHECK_NEAR(geom1.frame_rect.height, 638.0f, 1e-4);

        CHECK_NEAR(geom1.client_rect.x, 100.0f, 1e-4);
        CHECK_NEAR(geom1.client_rect.y, 150.0f, 1e-4);
        CHECK_NEAR(geom1.client_rect.width, 800.0f, 1e-4);
        CHECK_NEAR(geom1.client_rect.height, 600.0f, 1e-4);

        // Reverse calculation: from_frame must yield identical client rect
        FrameGeometry geom2 = compute_frame_geometry_from_frame(
            geom1.frame_rect, metrics, FrameState::Active, caption
        );
        CHECK_NEAR(geom2.client_rect.x, client_in.x, 1e-4);
        CHECK_NEAR(geom2.client_rect.y, client_in.y, 1e-4);
        CHECK_NEAR(geom2.client_rect.width, client_in.width, 1e-4);
        CHECK_NEAR(geom2.client_rect.height, client_in.height, 1e-4);

        // Caption rect
        CHECK_NEAR(geom1.caption_rect.x, 100.0f, 1e-4);
        CHECK_NEAR(geom1.caption_rect.y, 120.0f, 1e-4);
        CHECK_NEAR(geom1.caption_rect.width, 800.0f, 1e-4);
        CHECK_NEAR(geom1.caption_rect.height, 30.0f, 1e-4);

        // Corner radius
        CHECK_NEAR(geom1.corner_radius, 6.0f, 1e-4);
    }

    // 2. Button layout (Right-handed Windows/Linux style)
    {
        Rect client{0.0f, 0.0f, 600.0f, 400.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Active, caption
        );

        CHECK_EQ(geom.buttons.size(), 3u);
        // Buttons: Minimize, Maximize, Close
        CHECK_TRUE(geom.buttons[0].kind == ButtonKind::Minimize);
        CHECK_TRUE(geom.buttons[1].kind == ButtonKind::Maximize);
        CHECK_TRUE(geom.buttons[2].kind == ButtonKind::Close);

        // Close is rightmost
        const Rect& r_close = geom.buttons[2].rect;
        float right_bound = geom.caption_rect.right() - metrics.button_margin_right;
        CHECK_NEAR(r_close.right(), right_bound, 1e-4);
        CHECK_NEAR(r_close.width, 32.0f, 1e-4);
        CHECK_NEAR(r_close.height, 22.0f, 1e-4);

        // Maximize is to the left of Close with button_spacing
        const Rect& r_max = geom.buttons[1].rect;
        CHECK_NEAR(r_max.right(), r_close.x - metrics.button_spacing, 1e-4);

        // Minimize is to the left of Maximize with button_spacing
        const Rect& r_min = geom.buttons[0].rect;
        CHECK_NEAR(r_min.right(), r_max.x - metrics.button_spacing, 1e-4);
    }

    // 3. Button layout (Left-handed macOS style)
    {
        FrameMetrics mac_metrics = metrics;
        mac_metrics.left_handed_buttons = true;
        mac_metrics.buttons = {ButtonKind::Close, ButtonKind::Minimize, ButtonKind::Maximize};

        Rect client{0.0f, 0.0f, 600.0f, 400.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, mac_metrics, FrameState::Active, caption
        );

        CHECK_EQ(geom.buttons.size(), 3u);
        CHECK_TRUE(geom.buttons[0].kind == ButtonKind::Close);
        CHECK_TRUE(geom.buttons[1].kind == ButtonKind::Minimize);
        CHECK_TRUE(geom.buttons[2].kind == ButtonKind::Maximize);

        // Close is leftmost
        float left_bound = geom.caption_rect.x + mac_metrics.button_margin_left;
        CHECK_NEAR(geom.buttons[0].rect.x, left_bound, 1e-4);

        // Minimize follows Close
        CHECK_NEAR(geom.buttons[1].rect.x, geom.buttons[0].rect.right() + mac_metrics.button_spacing, 1e-4);

        // Maximize follows Minimize
        CHECK_NEAR(geom.buttons[2].rect.x, geom.buttons[1].rect.right() + mac_metrics.button_spacing, 1e-4);
    }

    // 4. Edge and Corner Grips
    {
        Rect client{50.0f, 50.0f, 500.0f, 300.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Active, caption
        );

        CHECK_FALSE(geom.grip_top_left.empty());
        CHECK_FALSE(geom.grip_top_right.empty());
        CHECK_FALSE(geom.grip_bottom_left.empty());
        CHECK_FALSE(geom.grip_bottom_right.empty());
        CHECK_FALSE(geom.grip_top.empty());
        CHECK_FALSE(geom.grip_bottom.empty());
        CHECK_FALSE(geom.grip_left.empty());
        CHECK_FALSE(geom.grip_right.empty());

        // Test grip_rect method
        CHECK_TRUE(geom.grip_rect(EdgeGrip::TopLeft) == geom.grip_top_left);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::TopRight) == geom.grip_top_right);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::BottomLeft) == geom.grip_bottom_left);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::BottomRight) == geom.grip_bottom_right);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::Top) == geom.grip_top);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::Bottom) == geom.grip_bottom);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::Left) == geom.grip_left);
        CHECK_TRUE(geom.grip_rect(EdgeGrip::Right) == geom.grip_right);
    }

    // 5. Maximized state
    {
        Rect client{0.0f, 0.0f, 1920.0f, 1050.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Maximized, caption
        );

        // Zero borders, zero corner radius
        CHECK_NEAR(geom.corner_radius, 0.0f, 1e-4);
        CHECK_NEAR(geom.border_margins.left, 0.0f, 1e-4);
        CHECK_NEAR(geom.border_margins.right, 0.0f, 1e-4);
        CHECK_NEAR(geom.border_margins.bottom, 0.0f, 1e-4);
        CHECK_NEAR(geom.border_margins.top, metrics.caption_height, 1e-4);

        // Maximize button should be replaced with Restore
        bool has_restore = false;
        bool has_maximize = false;
        for (const auto& b : geom.buttons) {
            if (b.kind == ButtonKind::Restore) has_restore = true;
            if (b.kind == ButtonKind::Maximize) has_maximize = true;
        }
        CHECK_TRUE(has_restore);
        CHECK_FALSE(has_maximize);

        // Grips disabled
        CHECK_TRUE(geom.grip_top_left.empty());
        CHECK_TRUE(geom.grip_top.empty());
        CHECK_TRUE(geom.hit_test_grip(Vec2{0.0f, 0.0f}) == EdgeGrip::None);

        // Shadow disabled
        CHECK_TRUE(geom.shadow_margins.empty());
    }

    // 6. Fullscreen state
    {
        Rect client{0.0f, 0.0f, 1920.0f, 1080.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Fullscreen, caption
        );

        CHECK_NEAR(geom.caption_rect.height, 0.0f, 1e-4);
        CHECK_NEAR(geom.corner_radius, 0.0f, 1e-4);
        CHECK_TRUE(geom.buttons.empty());
        CHECK_TRUE(geom.client_rect == geom.frame_rect);
        CHECK_TRUE(geom.shadow_margins.empty());
    }

    // 7. Tiled state
    {
        Rect client{0.0f, 0.0f, 960.0f, 1050.0f};
        FrameGeometry geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Tiled, caption
        );

        CHECK_NEAR(geom.corner_radius, 0.0f, 1e-4);
        CHECK_TRUE(geom.shadow_margins.empty());
        CHECK_TRUE(geom.caption_rect.height > 0.0f);
        CHECK_TRUE(geom.grip_top_left.empty());
    }

    return ::brotest::finish("test_frame_geometry");
}
