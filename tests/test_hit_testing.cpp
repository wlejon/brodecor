#include "check.h"
#include <brodecor/decor.h>

using namespace brodecor;

int main() {
    FrameMetrics metrics;
    metrics.caption_height = 30.0f;
    metrics.border_thickness = 4.0f;
    metrics.resize_grip_size = 8.0f;
    metrics.corner_radius = 6.0f;
    metrics.corner_grip_length = 24.0f;
    metrics.button_width = 30.0f;
    metrics.button_height = 20.0f;
    metrics.button_spacing = 4.0f;
    metrics.button_margin_right = 6.0f;
    metrics.shadow.size = 20.0f;
    metrics.shadow.offset = {0.0f, 4.0f};

    WindowCaption caption;
    caption.title = "Hit Test Window";

    Rect client{100.0f, 100.0f, 500.0f, 400.0f};
    FrameGeometry geom = compute_frame_geometry_from_client(
        client, metrics, FrameState::Active, caption
    );

    // 1. Hit test buttons
    {
        REQUIRE(geom.buttons.size() == 3u);

        // Close button (index 2)
        Vec2 close_center = geom.buttons[2].rect.center();
        HitTestResult res_close = geom.hit_test(close_center);
        CHECK_TRUE(res_close.is_button());
        CHECK_EQ(res_close.area, HitArea::Button);
        CHECK_TRUE(res_close.button.has_value());
        CHECK_EQ(*res_close.button, ButtonKind::Close);
        CHECK_EQ(res_close.button_index, 2);

        // Maximize button (index 1)
        Vec2 max_center = geom.buttons[1].rect.center();
        HitTestResult res_max = geom.hit_test(max_center);
        CHECK_TRUE(res_max.is_button());
        CHECK_EQ(*res_max.button, ButtonKind::Maximize);
        CHECK_EQ(res_max.button_index, 1);

        // Minimize button (index 0)
        Vec2 min_center = geom.buttons[0].rect.center();
        HitTestResult res_min = geom.hit_test(min_center);
        CHECK_TRUE(res_min.is_button());
        CHECK_EQ(*res_min.button, ButtonKind::Minimize);
        CHECK_EQ(res_min.button_index, 0);
    }

    // 2. Hit test caption (drag area)
    {
        // Point in titlebar away from buttons
        Vec2 caption_pt{geom.caption_rect.x + 50.0f, geom.caption_rect.center().y};
        HitTestResult res = geom.hit_test(caption_pt);
        CHECK_TRUE(res.is_caption());
        CHECK_TRUE(res.is_draggable());
        CHECK_EQ(res.area, HitArea::Caption);
        CHECK_FALSE(res.button.has_value());
    }

    // 3. Hit test client area
    {
        Vec2 client_pt = geom.client_rect.center();
        HitTestResult res = geom.hit_test(client_pt);
        CHECK_TRUE(res.is_client());
        CHECK_EQ(res.area, HitArea::Client);
        CHECK_FALSE(res.is_resizable());
        CHECK_FALSE(res.is_draggable());
    }

    // 4. Hit test corner grips
    {
        // Top-left corner
        Vec2 tl_pt{geom.frame_rect.x, geom.frame_rect.y};
        HitTestResult res_tl = geom.hit_test(tl_pt);
        CHECK_TRUE(res_tl.is_resizable());
        CHECK_EQ(res_tl.area, HitArea::ResizeBorder);
        CHECK_EQ(res_tl.edge, EdgeGrip::TopLeft);

        // Top-right corner (just left of right edge)
        Vec2 tr_pt{geom.frame_rect.right() - 2.0f, geom.frame_rect.y};
        HitTestResult res_tr = geom.hit_test(tr_pt);
        CHECK_TRUE(res_tr.is_resizable());
        CHECK_EQ(res_tr.edge, EdgeGrip::TopRight);

        // Bottom-left corner
        Vec2 bl_pt{geom.frame_rect.x, geom.frame_rect.bottom() - 2.0f};
        HitTestResult res_bl = geom.hit_test(bl_pt);
        CHECK_TRUE(res_bl.is_resizable());
        CHECK_EQ(res_bl.edge, EdgeGrip::BottomLeft);

        // Bottom-right corner
        Vec2 br_pt{geom.frame_rect.right() - 2.0f, geom.frame_rect.bottom() - 2.0f};
        HitTestResult res_br = geom.hit_test(br_pt);
        CHECK_TRUE(res_br.is_resizable());
        CHECK_EQ(res_br.edge, EdgeGrip::BottomRight);
    }

    // 5. Hit test edge grips
    {
        // Bottom edge middle
        Vec2 b_pt{geom.frame_rect.center().x, geom.frame_rect.bottom() - 1.0f};
        HitTestResult res_b = geom.hit_test(b_pt);
        CHECK_TRUE(res_b.is_resizable());
        CHECK_EQ(res_b.edge, EdgeGrip::Bottom);

        // Left edge middle
        Vec2 l_pt{geom.frame_rect.x, geom.frame_rect.center().y};
        HitTestResult res_l = geom.hit_test(l_pt);
        CHECK_TRUE(res_l.is_resizable());
        CHECK_EQ(res_l.edge, EdgeGrip::Left);

        // Right edge middle
        Vec2 r_pt{geom.frame_rect.right() - 1.0f, geom.frame_rect.center().y};
        HitTestResult res_r = geom.hit_test(r_pt);
        CHECK_TRUE(res_r.is_resizable());
        CHECK_EQ(res_r.edge, EdgeGrip::Right);
    }

    // 6. Hit test drop shadow
    {
        // Shadow area below window
        Vec2 shadow_pt{geom.frame_rect.center().x, geom.frame_rect.bottom() + 10.0f};
        HitTestResult res = geom.hit_test(shadow_pt);
        CHECK_TRUE(res.is_shadow());
        CHECK_EQ(res.area, HitArea::Shadow);
    }

    // 7. Hit test outside everything
    {
        Vec2 far_pt{0.0f, 0.0f};
        HitTestResult res = geom.hit_test(far_pt);
        CHECK_EQ(res.area, HitArea::None);
    }

    // 8. Maximized state: resize grips disabled
    {
        FrameGeometry max_geom = compute_frame_geometry_from_client(
            client, metrics, FrameState::Maximized, caption
        );

        // Edge point should not be resizable
        Vec2 edge_pt{max_geom.frame_rect.x, max_geom.frame_rect.center().y};
        HitTestResult res = max_geom.hit_test(edge_pt);
        CHECK_FALSE(res.is_resizable());
    }

    return ::brotest::finish("test_hit_testing");
}
