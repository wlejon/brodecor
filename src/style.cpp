#include "brodecor/style.h"

namespace brodecor {

Color ButtonStyle::background_for(ButtonState state, float hover_t, float press_t) const noexcept {
    if (state == ButtonState::Disabled) {
        return disabled_bg;
    }

    Color base = normal_bg;
    if (hover_t > 0.0f || state == ButtonState::Hover) {
        float t = (state == ButtonState::Hover && hover_t <= 0.0f) ? 1.0f : hover_t;
        base = bromath::clerp(base, hover_bg, t);
    }
    if (press_t > 0.0f || state == ButtonState::Pressed) {
        float t = (state == ButtonState::Pressed && press_t <= 0.0f) ? 1.0f : press_t;
        base = bromath::clerp(base, pressed_bg, t);
    }
    return base;
}

Color ButtonStyle::foreground_for(ButtonState state, float hover_t, float press_t) const noexcept {
    if (state == ButtonState::Disabled) {
        return disabled_fg;
    }

    Color base = normal_fg;
    if (hover_t > 0.0f || state == ButtonState::Hover) {
        float t = (state == ButtonState::Hover && hover_t <= 0.0f) ? 1.0f : hover_t;
        base = bromath::clerp(base, hover_fg, t);
    }
    if (press_t > 0.0f || state == ButtonState::Pressed) {
        float t = (state == ButtonState::Pressed && press_t <= 0.0f) ? 1.0f : press_t;
        base = bromath::clerp(base, pressed_fg, t);
    }
    return base;
}

FrameStyle FrameStyle::dark() noexcept {
    FrameColors c;
    c.titlebar_active_bg = Color(0.12f, 0.12f, 0.13f, 1.0f);
    c.titlebar_inactive_bg = Color(0.18f, 0.18f, 0.19f, 0.95f);
    c.title_active_fg = Color(0.96f, 0.96f, 0.96f, 1.0f);
    c.title_inactive_fg = Color(0.60f, 0.60f, 0.60f, 1.0f);
    c.subtitle_active_fg = Color(0.70f, 0.70f, 0.70f, 1.0f);
    c.subtitle_inactive_fg = Color(0.48f, 0.48f, 0.48f, 1.0f);
    c.border_active_color = Color(0.24f, 0.24f, 0.26f, 1.0f);
    c.border_inactive_color = Color(0.20f, 0.20f, 0.22f, 0.8f);
    c.accent_color = Color(0.0f, 0.47f, 0.83f, 1.0f);
    c.shadow_color = Color(0.0f, 0.0f, 0.0f, 0.45f);

    // Close button (active)
    c.close_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.close_button.hover_bg = Color(0.91f, 0.07f, 0.14f, 1.0f);    // #e81123
    c.close_button.pressed_bg = Color(0.72f, 0.05f, 0.11f, 1.0f);  // darker red
    c.close_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.close_button.normal_fg = Color(0.85f, 0.85f, 0.85f, 1.0f);
    c.close_button.hover_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.pressed_fg = Color(1.0f, 1.0f, 1.0f, 0.9f);
    c.close_button.disabled_fg = Color(0.40f, 0.40f, 0.40f, 0.5f);
    c.close_button.corner_radius = 4.0f;

    // Standard buttons (min/max/restore/menu)
    c.standard_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.standard_button.hover_bg = Color(1.0f, 1.0f, 1.0f, 0.10f);
    c.standard_button.pressed_bg = Color(1.0f, 1.0f, 1.0f, 0.20f);
    c.standard_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.standard_button.normal_fg = Color(0.85f, 0.85f, 0.85f, 1.0f);
    c.standard_button.hover_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.standard_button.pressed_fg = Color(1.0f, 1.0f, 1.0f, 0.9f);
    c.standard_button.disabled_fg = Color(0.40f, 0.40f, 0.40f, 0.5f);
    c.standard_button.corner_radius = 4.0f;

    // Inactive variants
    c.close_button_inactive = c.close_button;
    c.close_button_inactive.normal_fg = Color(0.55f, 0.55f, 0.55f, 1.0f);

    c.standard_button_inactive = c.standard_button;
    c.standard_button_inactive.normal_fg = Color(0.55f, 0.55f, 0.55f, 1.0f);

    return FrameStyle(c);
}

FrameStyle FrameStyle::light() noexcept {
    FrameColors c;
    c.titlebar_active_bg = Color(0.95f, 0.95f, 0.96f, 1.0f);
    c.titlebar_inactive_bg = Color(0.98f, 0.98f, 0.98f, 0.95f);
    c.title_active_fg = Color(0.08f, 0.08f, 0.08f, 1.0f);
    c.title_inactive_fg = Color(0.50f, 0.50f, 0.50f, 1.0f);
    c.subtitle_active_fg = Color(0.35f, 0.35f, 0.35f, 1.0f);
    c.subtitle_inactive_fg = Color(0.60f, 0.60f, 0.60f, 1.0f);
    c.border_active_color = Color(0.82f, 0.82f, 0.84f, 1.0f);
    c.border_inactive_color = Color(0.88f, 0.88f, 0.90f, 0.8f);
    c.accent_color = Color(0.0f, 0.40f, 0.72f, 1.0f);
    c.shadow_color = Color(0.0f, 0.0f, 0.0f, 0.22f);

    c.close_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.close_button.hover_bg = Color(0.91f, 0.07f, 0.14f, 1.0f);
    c.close_button.pressed_bg = Color(0.72f, 0.05f, 0.11f, 1.0f);
    c.close_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.close_button.normal_fg = Color(0.15f, 0.15f, 0.15f, 1.0f);
    c.close_button.hover_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.pressed_fg = Color(1.0f, 1.0f, 1.0f, 0.9f);
    c.close_button.disabled_fg = Color(0.65f, 0.65f, 0.65f, 0.5f);
    c.close_button.corner_radius = 4.0f;

    c.standard_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.standard_button.hover_bg = Color(0.0f, 0.0f, 0.0f, 0.07f);
    c.standard_button.pressed_bg = Color(0.0f, 0.0f, 0.0f, 0.15f);
    c.standard_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    c.standard_button.normal_fg = Color(0.15f, 0.15f, 0.15f, 1.0f);
    c.standard_button.hover_fg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.standard_button.pressed_fg = Color(0.0f, 0.0f, 0.0f, 0.9f);
    c.standard_button.disabled_fg = Color(0.65f, 0.65f, 0.65f, 0.5f);
    c.standard_button.corner_radius = 4.0f;

    c.close_button_inactive = c.close_button;
    c.close_button_inactive.normal_fg = Color(0.50f, 0.50f, 0.50f, 1.0f);

    c.standard_button_inactive = c.standard_button;
    c.standard_button_inactive.normal_fg = Color(0.50f, 0.50f, 0.50f, 1.0f);

    return FrameStyle(c);
}

FrameStyle FrameStyle::high_contrast() noexcept {
    FrameColors c;
    c.titlebar_active_bg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.titlebar_inactive_bg = Color(0.08f, 0.08f, 0.08f, 1.0f);
    c.title_active_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.title_inactive_fg = Color(0.70f, 0.70f, 0.70f, 1.0f);
    c.subtitle_active_fg = Color(0.0f, 1.0f, 1.0f, 1.0f); // cyan
    c.subtitle_inactive_fg = Color(0.50f, 0.50f, 0.50f, 1.0f);
    c.border_active_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.border_inactive_color = Color(0.50f, 0.50f, 0.50f, 1.0f);
    c.accent_color = Color(1.0f, 1.0f, 0.0f, 1.0f); // yellow
    c.shadow_color = Color(0.0f, 0.0f, 0.0f, 0.0f); // no shadow in high contrast

    c.close_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.close_button.hover_bg = Color(1.0f, 0.0f, 0.0f, 1.0f);
    c.close_button.pressed_bg = Color(0.70f, 0.0f, 0.0f, 1.0f);
    c.close_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.close_button.normal_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.hover_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.pressed_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.disabled_fg = Color(0.40f, 0.40f, 0.40f, 1.0f);
    c.close_button.border_width = 1.0f;
    c.close_button.border_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.close_button.corner_radius = 0.0f;

    c.standard_button.normal_bg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.standard_button.hover_bg = Color(0.20f, 0.20f, 0.20f, 1.0f);
    c.standard_button.pressed_bg = Color(0.40f, 0.40f, 0.40f, 1.0f);
    c.standard_button.disabled_bg = Color(0.0f, 0.0f, 0.0f, 1.0f);
    c.standard_button.normal_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.standard_button.hover_fg = Color(1.0f, 1.0f, 0.0f, 1.0f);
    c.standard_button.pressed_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.standard_button.disabled_fg = Color(0.40f, 0.40f, 0.40f, 1.0f);
    c.standard_button.border_width = 1.0f;
    c.standard_button.border_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    c.standard_button.corner_radius = 0.0f;

    c.close_button_inactive = c.close_button;
    c.close_button_inactive.border_color = Color(0.50f, 0.50f, 0.50f, 1.0f);
    c.close_button_inactive.normal_fg = Color(0.70f, 0.70f, 0.70f, 1.0f);

    c.standard_button_inactive = c.standard_button;
    c.standard_button_inactive.border_color = Color(0.50f, 0.50f, 0.50f, 1.0f);
    c.standard_button_inactive.normal_fg = Color(0.70f, 0.70f, 0.70f, 1.0f);

    return FrameStyle(c);
}

Color FrameStyle::titlebar_bg(FrameState state) const noexcept {
    return is_inactive(state) ? colors.titlebar_inactive_bg : colors.titlebar_active_bg;
}

Color FrameStyle::title_fg(FrameState state) const noexcept {
    return is_inactive(state) ? colors.title_inactive_fg : colors.title_active_fg;
}

Color FrameStyle::subtitle_fg(FrameState state) const noexcept {
    return is_inactive(state) ? colors.subtitle_inactive_fg : colors.subtitle_active_fg;
}

Color FrameStyle::border_color(FrameState state) const noexcept {
    return is_inactive(state) ? colors.border_inactive_color : colors.border_active_color;
}

Color FrameStyle::shadow_color(FrameState state) const noexcept {
    if (is_maximized(state) || is_fullscreen(state) || is_tiled(state)) {
        return Color(0.0f, 0.0f, 0.0f, 0.0f);
    }
    if (is_inactive(state)) {
        // Reduced shadow opacity for inactive windows
        Color sc = colors.shadow_color;
        sc.a *= 0.6f;
        return sc;
    }
    return colors.shadow_color;
}

const ButtonStyle& FrameStyle::button_style(ButtonKind kind, FrameState state) const noexcept {
    bool inactive = is_inactive(state);
    if (kind == ButtonKind::Close) {
        return inactive ? colors.close_button_inactive : colors.close_button;
    }
    return inactive ? colors.standard_button_inactive : colors.standard_button;
}

static ButtonStyle lerp_btn_style(const ButtonStyle& a, const ButtonStyle& b, float t) noexcept {
    ButtonStyle r;
    r.normal_bg = bromath::clerp(a.normal_bg, b.normal_bg, t);
    r.hover_bg = bromath::clerp(a.hover_bg, b.hover_bg, t);
    r.pressed_bg = bromath::clerp(a.pressed_bg, b.pressed_bg, t);
    r.disabled_bg = bromath::clerp(a.disabled_bg, b.disabled_bg, t);
    r.normal_fg = bromath::clerp(a.normal_fg, b.normal_fg, t);
    r.hover_fg = bromath::clerp(a.hover_fg, b.hover_fg, t);
    r.pressed_fg = bromath::clerp(a.pressed_fg, b.pressed_fg, t);
    r.disabled_fg = bromath::clerp(a.disabled_fg, b.disabled_fg, t);
    r.corner_radius = bromath::lerp(a.corner_radius, b.corner_radius, t);
    r.icon_size = bromath::lerp(a.icon_size, b.icon_size, t);
    r.border_width = bromath::lerp(a.border_width, b.border_width, t);
    r.border_color = bromath::clerp(a.border_color, b.border_color, t);
    return r;
}

FrameColors FrameStyle::lerp_colors(const FrameColors& a, const FrameColors& b, float t) noexcept {
    FrameColors r;
    r.titlebar_active_bg = bromath::clerp(a.titlebar_active_bg, b.titlebar_active_bg, t);
    r.titlebar_inactive_bg = bromath::clerp(a.titlebar_inactive_bg, b.titlebar_inactive_bg, t);
    r.title_active_fg = bromath::clerp(a.title_active_fg, b.title_active_fg, t);
    r.title_inactive_fg = bromath::clerp(a.title_inactive_fg, b.title_inactive_fg, t);
    r.subtitle_active_fg = bromath::clerp(a.subtitle_active_fg, b.subtitle_active_fg, t);
    r.subtitle_inactive_fg = bromath::clerp(a.subtitle_inactive_fg, b.subtitle_inactive_fg, t);
    r.border_active_color = bromath::clerp(a.border_active_color, b.border_active_color, t);
    r.border_inactive_color = bromath::clerp(a.border_inactive_color, b.border_inactive_color, t);
    r.accent_color = bromath::clerp(a.accent_color, b.accent_color, t);
    r.shadow_color = bromath::clerp(a.shadow_color, b.shadow_color, t);
    r.close_button = lerp_btn_style(a.close_button, b.close_button, t);
    r.standard_button = lerp_btn_style(a.standard_button, b.standard_button, t);
    r.close_button_inactive = lerp_btn_style(a.close_button_inactive, b.close_button_inactive, t);
    r.standard_button_inactive = lerp_btn_style(a.standard_button_inactive, b.standard_button_inactive, t);
    return r;
}

} // namespace brodecor
