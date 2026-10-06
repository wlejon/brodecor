#pragma once

#include "brodecor/types.h"

namespace brodecor {

struct ButtonStyle {
    Color normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
    Color hover_bg = Color(1.0f, 1.0f, 1.0f, 0.12f);
    Color pressed_bg = Color(1.0f, 1.0f, 1.0f, 0.22f);
    Color disabled_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);

    Color normal_fg = Color(0.85f, 0.85f, 0.85f, 1.0f);
    Color hover_fg = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Color pressed_fg = Color(1.0f, 1.0f, 1.0f, 0.9f);
    Color disabled_fg = Color(0.45f, 0.45f, 0.45f, 0.6f);

    float corner_radius = 4.0f;
    float icon_size = 12.0f;
    float border_width = 0.0f;
    Color border_color = Color(0.0f, 0.0f, 0.0f, 0.0f);

    Color background_for(ButtonState state, float hover_t = 0.0f, float press_t = 0.0f) const noexcept;
    Color foreground_for(ButtonState state, float hover_t = 0.0f, float press_t = 0.0f) const noexcept;
};

struct FrameColors {
    Color titlebar_active_bg;
    Color titlebar_inactive_bg;
    Color title_active_fg;
    Color title_inactive_fg;
    Color subtitle_active_fg;
    Color subtitle_inactive_fg;
    Color border_active_color;
    Color border_inactive_color;
    Color accent_color;
    Color shadow_color;

    ButtonStyle close_button;
    ButtonStyle standard_button;
    ButtonStyle close_button_inactive;
    ButtonStyle standard_button_inactive;
};

class FrameStyle {
public:
    FrameColors colors;

    FrameStyle() = default;
    explicit FrameStyle(const FrameColors& c) : colors(c) {}

    static FrameStyle dark() noexcept;
    static FrameStyle light() noexcept;
    static FrameStyle high_contrast() noexcept;

    Color titlebar_bg(FrameState state) const noexcept;
    Color title_fg(FrameState state) const noexcept;
    Color subtitle_fg(FrameState state) const noexcept;
    Color border_color(FrameState state) const noexcept;
    Color shadow_color(FrameState state) const noexcept;
    const ButtonStyle& button_style(ButtonKind kind, FrameState state) const noexcept;

    static FrameColors lerp_colors(const FrameColors& a, const FrameColors& b, float t) noexcept;
};

} // namespace brodecor
