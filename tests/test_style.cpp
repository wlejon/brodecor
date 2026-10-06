#include "check.h"
#include <brodecor/style.h>

using namespace brodecor;

int main() {
    // 1. Dark theme presets
    {
        FrameStyle dark = FrameStyle::dark();

        Color active_bg = dark.titlebar_bg(FrameState::Active);
        Color inactive_bg = dark.titlebar_bg(FrameState::Inactive);
        CHECK_FALSE(active_bg == inactive_bg);

        Color active_title = dark.title_fg(FrameState::Active);
        CHECK_NEAR(active_title.r, 0.96f, 0.05);

        // Close button hover color is vivid red
        const ButtonStyle& close_btn = dark.button_style(ButtonKind::Close, FrameState::Active);
        CHECK_NEAR(close_btn.hover_bg.r, 0.91f, 0.05);

        // Shadow color active vs inactive vs maximized
        Color shadow_act = dark.shadow_color(FrameState::Active);
        CHECK_TRUE(shadow_act.a > 0.0f);

        Color shadow_inact = dark.shadow_color(FrameState::Inactive);
        CHECK_TRUE(shadow_inact.a < shadow_act.a);

        Color shadow_max = dark.shadow_color(FrameState::Maximized);
        CHECK_NEAR(shadow_max.a, 0.0f, 1e-4);
    }

    // 2. Light theme presets
    {
        FrameStyle light = FrameStyle::light();

        Color title_fg = light.title_fg(FrameState::Active);
        // Dark text on light background
        CHECK_TRUE(title_fg.r < 0.2f);

        Color title_bg = light.titlebar_bg(FrameState::Active);
        CHECK_TRUE(title_bg.r > 0.9f);
    }

    // 3. High contrast presets
    {
        FrameStyle hc = FrameStyle::high_contrast();

        Color title_bg = hc.titlebar_bg(FrameState::Active);
        CHECK_NEAR(title_bg.r, 0.0f, 1e-4);

        Color border = hc.border_color(FrameState::Active);
        CHECK_NEAR(border.r, 1.0f, 1e-4);

        Color shadow = hc.shadow_color(FrameState::Active);
        CHECK_NEAR(shadow.a, 0.0f, 1e-4); // No shadows in high contrast

        const ButtonStyle& btn = hc.button_style(ButtonKind::Minimize, FrameState::Active);
        CHECK_NEAR(btn.corner_radius, 0.0f, 1e-4);
        CHECK_TRUE(btn.border_width > 0.0f);
    }

    // 4. ButtonStyle state & progress evaluation
    {
        ButtonStyle style;
        style.normal_bg = Color(0.0f, 0.0f, 0.0f, 0.0f);
        style.hover_bg = Color(1.0f, 1.0f, 1.0f, 0.2f);
        style.pressed_bg = Color(1.0f, 1.0f, 1.0f, 0.4f);
        style.disabled_bg = Color(0.1f, 0.1f, 0.1f, 0.1f);

        // Normal
        Color c_norm = style.background_for(ButtonState::Normal, 0.0f, 0.0f);
        CHECK_NEAR(c_norm.a, 0.0f, 1e-4);

        // Partial hover
        Color c_hover_half = style.background_for(ButtonState::Normal, 0.5f, 0.0f);
        CHECK_NEAR(c_hover_half.a, 0.1f, 1e-3);

        // Full hover
        Color c_hover = style.background_for(ButtonState::Hover, 1.0f, 0.0f);
        CHECK_NEAR(c_hover.a, 0.2f, 1e-3);

        // Full pressed
        Color c_press = style.background_for(ButtonState::Pressed, 1.0f, 1.0f);
        CHECK_NEAR(c_press.a, 0.4f, 1e-3);

        // Disabled
        Color c_dis = style.background_for(ButtonState::Disabled, 1.0f, 1.0f);
        CHECK_NEAR(c_dis.a, 0.1f, 1e-3);
    }

    // 5. FrameStyle lerp
    {
        FrameStyle dark = FrameStyle::dark();
        FrameStyle light = FrameStyle::light();

        FrameColors mid = FrameStyle::lerp_colors(dark.colors, light.colors, 0.5f);
        float expected_r = (dark.colors.titlebar_active_bg.r + light.colors.titlebar_active_bg.r) * 0.5f;
        CHECK_NEAR(mid.titlebar_active_bg.r, expected_r, 1e-3);
    }

    return ::brotest::finish("test_style");
}
