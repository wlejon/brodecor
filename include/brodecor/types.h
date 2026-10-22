#pragma once

#include <bromath/aabb.h>
#include <bromath/color.h>
#include <bromath/vec.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace brodecor {

using bromath::Vec2;
using bromath::Color;
using bromath::Color8;

inline constexpr bool operator==(const bromath::Vec2& a, const bromath::Vec2& b) noexcept {
    return a.x == b.x && a.y == b.y;
}

inline constexpr bool operator==(const bromath::Color& a, const bromath::Color& b) noexcept {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

// --- 2D Geometry primitives ---

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rect() = default;
    constexpr Rect(float x_, float y_, float w_, float h_)
        : x(x_), y(y_), width(w_), height(h_) {}

    constexpr float left() const noexcept { return x; }
    constexpr float top() const noexcept { return y; }
    constexpr float right() const noexcept { return x + width; }
    constexpr float bottom() const noexcept { return y + height; }
    constexpr Vec2 pos() const noexcept { return {x, y}; }
    constexpr Vec2 size() const noexcept { return {width, height}; }
    constexpr Vec2 center() const noexcept { return {x + width * 0.5f, y + height * 0.5f}; }

    constexpr bool empty() const noexcept { return width <= 0.0f || height <= 0.0f; }

    constexpr bool contains(Vec2 p) const noexcept {
        return p.x >= x && p.x < right() && p.y >= y && p.y < bottom();
    }

    constexpr bool contains(float px, float py) const noexcept {
        return contains(Vec2{px, py});
    }

    constexpr bool contains(const Rect& r) const noexcept {
        return r.x >= x && r.right() <= right() && r.y >= y && r.bottom() <= bottom();
    }

    constexpr bool intersects(const Rect& r) const noexcept {
        return !empty() && !r.empty() &&
               x < r.right() && right() > r.x &&
               y < r.bottom() && bottom() > r.y;
    }

    constexpr Rect intersected(const Rect& r) const noexcept {
        if (empty() || r.empty()) return Rect{0.0f, 0.0f, 0.0f, 0.0f};
        float l = std::max(x, r.x);
        float t = std::max(y, r.y);
        float r_edge = std::min(right(), r.right());
        float b_edge = std::min(bottom(), r.bottom());
        if (r_edge <= l || b_edge <= t) return Rect{0.0f, 0.0f, 0.0f, 0.0f};
        return Rect{l, t, r_edge - l, b_edge - t};
    }

    constexpr Rect united(const Rect& r) const noexcept {
        if (empty()) return r;
        if (r.empty()) return *this;
        float l = std::min(x, r.x);
        float t = std::min(y, r.y);
        float r_edge = std::max(right(), r.right());
        float b_edge = std::max(bottom(), r.bottom());
        return Rect{l, t, r_edge - l, b_edge - t};
    }

    constexpr Rect inflated(float dx, float dy) const noexcept {
        return Rect{x - dx, y - dy, width + 2.0f * dx, height + 2.0f * dy};
    }

    constexpr Rect offset(float dx, float dy) const noexcept {
        return Rect{x + dx, y + dy, width, height};
    }

    constexpr Rect offset(Vec2 d) const noexcept {
        return Rect{x + d.x, y + d.y, width, height};
    }

    constexpr bromath::AABB2 to_aabb() const noexcept {
        return bromath::AABB2{{x, y}, {x + width, y + height}};
    }

    static constexpr Rect from_aabb(const bromath::AABB2& bb) noexcept {
        return Rect{bb.min.x, bb.min.y, bb.max.x - bb.min.x, bb.max.y - bb.min.y};
    }

    constexpr bool operator==(const Rect& o) const noexcept = default;
};

struct Margins {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    constexpr Margins() = default;
    constexpr Margins(float l, float t, float r, float b) noexcept
        : left(l), top(t), right(r), bottom(b) {}
    constexpr explicit Margins(float uniform) noexcept
        : left(uniform), top(uniform), right(uniform), bottom(uniform) {}

    constexpr float horizontal() const noexcept { return left + right; }
    constexpr float vertical() const noexcept { return top + bottom; }
    constexpr bool empty() const noexcept {
        return left == 0.0f && top == 0.0f && right == 0.0f && bottom == 0.0f;
    }

    constexpr bool operator==(const Margins& o) const noexcept = default;
};

// --- Button Types ---

enum class ButtonKind : uint8_t {
    Close,
    Minimize,
    Maximize,
    Restore,
    Menu,
    Custom
};

inline const char* to_string(ButtonKind kind) noexcept {
    switch (kind) {
        case ButtonKind::Close: return "Close";
        case ButtonKind::Minimize: return "Minimize";
        case ButtonKind::Maximize: return "Maximize";
        case ButtonKind::Restore: return "Restore";
        case ButtonKind::Menu: return "Menu";
        case ButtonKind::Custom: return "Custom";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, ButtonKind kind) {
    return os << to_string(kind);
}

constexpr bool is_maximize_or_restore(ButtonKind k) noexcept {
    return k == ButtonKind::Maximize || k == ButtonKind::Restore;
}

enum class ButtonState : uint8_t {
    Normal,
    Hover,
    Pressed,
    Disabled
};

inline const char* to_string(ButtonState state) noexcept {
    switch (state) {
        case ButtonState::Normal: return "Normal";
        case ButtonState::Hover: return "Hover";
        case ButtonState::Pressed: return "Pressed";
        case ButtonState::Disabled: return "Disabled";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, ButtonState state) {
    return os << to_string(state);
}

// --- Edge Grip Types ---

enum class EdgeGrip : uint8_t {
    None = 0,
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

inline const char* to_string(EdgeGrip grip) noexcept {
    switch (grip) {
        case EdgeGrip::None: return "None";
        case EdgeGrip::Top: return "Top";
        case EdgeGrip::Bottom: return "Bottom";
        case EdgeGrip::Left: return "Left";
        case EdgeGrip::Right: return "Right";
        case EdgeGrip::TopLeft: return "TopLeft";
        case EdgeGrip::TopRight: return "TopRight";
        case EdgeGrip::BottomLeft: return "BottomLeft";
        case EdgeGrip::BottomRight: return "BottomRight";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, EdgeGrip grip) {
    return os << to_string(grip);
}

inline const char* cursor_name(EdgeGrip grip) noexcept {
    switch (grip) {
        case EdgeGrip::None: return "default";
        case EdgeGrip::Top: return "n-resize";
        case EdgeGrip::Bottom: return "s-resize";
        case EdgeGrip::Left: return "w-resize";
        case EdgeGrip::Right: return "e-resize";
        case EdgeGrip::TopLeft: return "nw-resize";
        case EdgeGrip::TopRight: return "ne-resize";
        case EdgeGrip::BottomLeft: return "sw-resize";
        case EdgeGrip::BottomRight: return "se-resize";
    }
    return "default";
}

constexpr bool is_corner(EdgeGrip grip) noexcept {
    return grip == EdgeGrip::TopLeft || grip == EdgeGrip::TopRight ||
           grip == EdgeGrip::BottomLeft || grip == EdgeGrip::BottomRight;
}

constexpr bool is_edge(EdgeGrip grip) noexcept {
    return grip == EdgeGrip::Top || grip == EdgeGrip::Bottom ||
           grip == EdgeGrip::Left || grip == EdgeGrip::Right;
}

// --- Frame State Flags ---

enum class FrameState : uint32_t {
    Normal     = 0,
    Active     = 1 << 0,
    Inactive   = 1 << 1,
    Maximized  = 1 << 2,
    Fullscreen = 1 << 3,
    Tiled      = 1 << 4,
};

constexpr FrameState operator|(FrameState a, FrameState b) noexcept {
    return static_cast<FrameState>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

constexpr FrameState operator&(FrameState a, FrameState b) noexcept {
    return static_cast<FrameState>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

constexpr FrameState operator^(FrameState a, FrameState b) noexcept {
    return static_cast<FrameState>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

constexpr FrameState operator~(FrameState a) noexcept {
    return static_cast<FrameState>(~static_cast<uint32_t>(a));
}

constexpr FrameState& operator|=(FrameState& a, FrameState b) noexcept {
    a = a | b;
    return a;
}

constexpr FrameState& operator&=(FrameState& a, FrameState b) noexcept {
    a = a & b;
    return a;
}

constexpr bool has_state(FrameState state, FrameState flag) noexcept {
    return (static_cast<uint32_t>(state) & static_cast<uint32_t>(flag)) != 0;
}

constexpr bool is_active(FrameState s) noexcept {
    return has_state(s, FrameState::Active) || (!has_state(s, FrameState::Inactive) && s == FrameState::Normal);
}

constexpr bool is_inactive(FrameState s) noexcept {
    return has_state(s, FrameState::Inactive);
}

constexpr bool is_maximized(FrameState s) noexcept {
    return has_state(s, FrameState::Maximized);
}

constexpr bool is_fullscreen(FrameState s) noexcept {
    return has_state(s, FrameState::Fullscreen);
}

constexpr bool is_tiled(FrameState s) noexcept {
    return has_state(s, FrameState::Tiled);
}

inline const char* to_string(FrameState state) noexcept {
    if (is_fullscreen(state)) return "fullscreen";
    if (is_maximized(state)) return "maximized";
    if (is_tiled(state)) return "tiled";
    if (is_inactive(state)) return "inactive";
    return "active";
}

// --- Window Caption ---

struct WindowCaption {
    std::string title;
    std::string subtitle;
    bool show_icon = true;
    bool show_title = true;
    bool centered = false;

    bool empty() const noexcept { return title.empty(); }
};

} // namespace brodecor
