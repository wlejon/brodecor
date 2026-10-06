#include "check.h"
#include <brodecor/types.h>

using namespace brodecor;

int main() {
    // 1. Rect construction & basic properties
    {
        Rect r{10.0f, 20.0f, 100.0f, 50.0f};
        CHECK_NEAR(r.left(), 10.0f, 1e-5);
        CHECK_NEAR(r.top(), 20.0f, 1e-5);
        CHECK_NEAR(r.right(), 110.0f, 1e-5);
        CHECK_NEAR(r.bottom(), 70.0f, 1e-5);

        CHECK_NEAR(r.pos().x, 10.0f, 1e-5);
        CHECK_NEAR(r.pos().y, 20.0f, 1e-5);
        CHECK_NEAR(r.size().x, 100.0f, 1e-5);
        CHECK_NEAR(r.size().y, 50.0f, 1e-5);
        CHECK_NEAR(r.center().x, 60.0f, 1e-5);
        CHECK_NEAR(r.center().y, 45.0f, 1e-5);

        CHECK_FALSE(r.empty());

        Rect empty_r{0.0f, 0.0f, 0.0f, 10.0f};
        CHECK_TRUE(empty_r.empty());
    }

    // 2. Rect contains & intersects
    {
        Rect r{10.0f, 20.0f, 100.0f, 50.0f};

        CHECK_TRUE(r.contains(Vec2{10.0f, 20.0f}));
        CHECK_TRUE(r.contains(Vec2{50.0f, 40.0f}));
        CHECK_TRUE(r.contains(Vec2{109.9f, 69.9f}));
        CHECK_FALSE(r.contains(Vec2{110.0f, 70.0f})); // right/bottom exclusive
        CHECK_FALSE(r.contains(Vec2{9.0f, 20.0f}));

        CHECK_TRUE(r.contains(10.0f, 20.0f));
        CHECK_FALSE(r.contains(120.0f, 40.0f));

        Rect inner{20.0f, 30.0f, 50.0f, 20.0f};
        CHECK_TRUE(r.contains(inner));

        Rect overlapping{50.0f, 40.0f, 100.0f, 50.0f};
        CHECK_TRUE(r.intersects(overlapping));
        CHECK_FALSE(r.contains(overlapping));

        Rect outside{200.0f, 200.0f, 50.0f, 50.0f};
        CHECK_FALSE(r.intersects(outside));
    }

    // 3. Rect operations (intersection, union, inflation, offset)
    {
        Rect a{0.0f, 0.0f, 100.0f, 100.0f};
        Rect b{50.0f, 50.0f, 100.0f, 100.0f};

        Rect inter = a.intersected(b);
        CHECK_NEAR(inter.x, 50.0f, 1e-5);
        CHECK_NEAR(inter.y, 50.0f, 1e-5);
        CHECK_NEAR(inter.width, 50.0f, 1e-5);
        CHECK_NEAR(inter.height, 50.0f, 1e-5);

        Rect un = a.united(b);
        CHECK_NEAR(un.x, 0.0f, 1e-5);
        CHECK_NEAR(un.y, 0.0f, 1e-5);
        CHECK_NEAR(un.width, 150.0f, 1e-5);
        CHECK_NEAR(un.height, 150.0f, 1e-5);

        Rect inf = a.inflated(5.0f, 10.0f);
        CHECK_NEAR(inf.x, -5.0f, 1e-5);
        CHECK_NEAR(inf.y, -10.0f, 1e-5);
        CHECK_NEAR(inf.width, 110.0f, 1e-5);
        CHECK_NEAR(inf.height, 120.0f, 1e-5);

        Rect off = a.offset(10.0f, -5.0f);
        CHECK_NEAR(off.x, 10.0f, 1e-5);
        CHECK_NEAR(off.y, -5.0f, 1e-5);
        CHECK_NEAR(off.width, 100.0f, 1e-5);
        CHECK_NEAR(off.height, 100.0f, 1e-5);
    }

    // 4. Rect <-> bromath::AABB2 conversion
    {
        Rect r{10.0f, 20.0f, 80.0f, 60.0f};
        bromath::AABB2 aabb = r.to_aabb();
        CHECK_NEAR(aabb.min.x, 10.0f, 1e-5);
        CHECK_NEAR(aabb.min.y, 20.0f, 1e-5);
        CHECK_NEAR(aabb.max.x, 90.0f, 1e-5);
        CHECK_NEAR(aabb.max.y, 80.0f, 1e-5);

        Rect from = Rect::from_aabb(aabb);
        CHECK_TRUE(r == from);
    }

    // 5. Margins
    {
        Margins m{4.0f, 8.0f, 6.0f, 10.0f};
        CHECK_NEAR(m.horizontal(), 10.0f, 1e-5);
        CHECK_NEAR(m.vertical(), 18.0f, 1e-5);
        CHECK_FALSE(m.empty());

        Margins uniform{5.0f};
        CHECK_NEAR(uniform.left, 5.0f, 1e-5);
        CHECK_NEAR(uniform.top, 5.0f, 1e-5);
        CHECK_NEAR(uniform.right, 5.0f, 1e-5);
        CHECK_NEAR(uniform.bottom, 5.0f, 1e-5);

        Margins zero{};
        CHECK_TRUE(zero.empty());
    }

    // 6. ButtonKind & ButtonState
    {
        CHECK_TRUE(is_maximize_or_restore(ButtonKind::Maximize));
        CHECK_TRUE(is_maximize_or_restore(ButtonKind::Restore));
        CHECK_FALSE(is_maximize_or_restore(ButtonKind::Close));
        CHECK_FALSE(is_maximize_or_restore(ButtonKind::Minimize));
        CHECK_FALSE(is_maximize_or_restore(ButtonKind::Menu));

        CHECK_EQ(std::string(to_string(ButtonKind::Close)), "Close");
        CHECK_EQ(std::string(to_string(ButtonKind::Minimize)), "Minimize");
        CHECK_EQ(std::string(to_string(ButtonKind::Maximize)), "Maximize");
        CHECK_EQ(std::string(to_string(ButtonKind::Restore)), "Restore");
        CHECK_EQ(std::string(to_string(ButtonKind::Menu)), "Menu");

        CHECK_EQ(std::string(to_string(ButtonState::Normal)), "Normal");
        CHECK_EQ(std::string(to_string(ButtonState::Hover)), "Hover");
        CHECK_EQ(std::string(to_string(ButtonState::Pressed)), "Pressed");
        CHECK_EQ(std::string(to_string(ButtonState::Disabled)), "Disabled");
    }

    // 7. EdgeGrip
    {
        CHECK_TRUE(is_corner(EdgeGrip::TopLeft));
        CHECK_TRUE(is_corner(EdgeGrip::TopRight));
        CHECK_TRUE(is_corner(EdgeGrip::BottomLeft));
        CHECK_TRUE(is_corner(EdgeGrip::BottomRight));
        CHECK_FALSE(is_corner(EdgeGrip::Top));
        CHECK_FALSE(is_corner(EdgeGrip::None));

        CHECK_TRUE(is_edge(EdgeGrip::Top));
        CHECK_TRUE(is_edge(EdgeGrip::Bottom));
        CHECK_TRUE(is_edge(EdgeGrip::Left));
        CHECK_TRUE(is_edge(EdgeGrip::Right));
        CHECK_FALSE(is_edge(EdgeGrip::TopLeft));
        CHECK_FALSE(is_edge(EdgeGrip::None));

        CHECK_EQ(std::string(cursor_name(EdgeGrip::Top)), "n-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::Bottom)), "s-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::Left)), "w-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::Right)), "e-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::TopLeft)), "nw-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::TopRight)), "ne-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::BottomLeft)), "sw-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::BottomRight)), "se-resize");
        CHECK_EQ(std::string(cursor_name(EdgeGrip::None)), "default");
    }

    // 8. FrameState bitwise flags & helpers
    {
        FrameState s = FrameState::Active | FrameState::Maximized;
        CHECK_TRUE(is_active(s));
        CHECK_TRUE(is_maximized(s));
        CHECK_FALSE(is_inactive(s));
        CHECK_FALSE(is_fullscreen(s));
        CHECK_FALSE(is_tiled(s));

        FrameState inactive_normal = FrameState::Inactive;
        CHECK_TRUE(is_inactive(inactive_normal));
        CHECK_FALSE(is_active(inactive_normal));

        FrameState default_normal = FrameState::Normal;
        CHECK_TRUE(is_active(default_normal)); // default normal is considered active
        CHECK_FALSE(is_inactive(default_normal));

        FrameState tiled = FrameState::Active | FrameState::Tiled;
        CHECK_TRUE(is_tiled(tiled));
        CHECK_TRUE(is_active(tiled));

        FrameState fs = FrameState::Fullscreen;
        CHECK_TRUE(is_fullscreen(fs));
    }

    // 9. WindowCaption
    {
        WindowCaption cap;
        CHECK_TRUE(cap.empty());
        cap.title = "brodecor demo";
        CHECK_FALSE(cap.empty());
        CHECK_TRUE(cap.show_icon);
        CHECK_TRUE(cap.show_title);
        CHECK_FALSE(cap.centered);
    }

    return ::brotest::finish("test_types");
}
