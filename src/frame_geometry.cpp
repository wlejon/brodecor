#include "brodecor/decor.h"

#include <algorithm>

namespace brodecor {

Margins FrameMetrics::effective_borders(FrameState state) const noexcept {
    if (is_fullscreen(state) || is_maximized(state)) {
        return Margins{0.0f, 0.0f, 0.0f, 0.0f};
    }
    if (is_tiled(state)) {
        float t = std::min(border_thickness, 1.0f);
        return Margins{t, t, t, t};
    }
    return Margins{border_thickness, border_thickness, border_thickness, border_thickness};
}

float FrameMetrics::effective_caption_height(FrameState state) const noexcept {
    if (is_fullscreen(state)) {
        return 0.0f;
    }
    return caption_height;
}

float FrameMetrics::effective_corner_radius(FrameState state) const noexcept {
    if (is_fullscreen(state) || is_maximized(state) || is_tiled(state)) {
        return 0.0f;
    }
    return corner_radius;
}

Rect FrameGeometry::grip_rect(EdgeGrip grip) const noexcept {
    switch (grip) {
        case EdgeGrip::None: return Rect{0.0f, 0.0f, 0.0f, 0.0f};
        case EdgeGrip::Top: return grip_top;
        case EdgeGrip::Bottom: return grip_bottom;
        case EdgeGrip::Left: return grip_left;
        case EdgeGrip::Right: return grip_right;
        case EdgeGrip::TopLeft: return grip_top_left;
        case EdgeGrip::TopRight: return grip_top_right;
        case EdgeGrip::BottomLeft: return grip_bottom_left;
        case EdgeGrip::BottomRight: return grip_bottom_right;
    }
    return Rect{0.0f, 0.0f, 0.0f, 0.0f};
}

EdgeGrip FrameGeometry::hit_test_grip(Vec2 point) const noexcept {
    if (is_maximized(state) || is_fullscreen(state) || is_tiled(state)) {
        return EdgeGrip::None;
    }

    // Corners have higher priority than edges
    if (grip_top_left.contains(point)) return EdgeGrip::TopLeft;
    if (grip_top_right.contains(point)) return EdgeGrip::TopRight;
    if (grip_bottom_left.contains(point)) return EdgeGrip::BottomLeft;
    if (grip_bottom_right.contains(point)) return EdgeGrip::BottomRight;

    if (grip_top.contains(point)) return EdgeGrip::Top;
    if (grip_bottom.contains(point)) return EdgeGrip::Bottom;
    if (grip_left.contains(point)) return EdgeGrip::Left;
    if (grip_right.contains(point)) return EdgeGrip::Right;

    return EdgeGrip::None;
}

std::optional<ButtonKind> FrameGeometry::hit_test_button(Vec2 point, int* out_index) const noexcept {
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i].rect.contains(point)) {
            if (out_index) *out_index = static_cast<int>(i);
            return buttons[i].kind;
        }
    }
    if (out_index) *out_index = -1;
    return std::nullopt;
}

HitTestResult FrameGeometry::hit_test(Vec2 point) const noexcept {
    HitTestResult res;

    // 1. Buttons take top priority
    int btn_idx = -1;
    auto btn = hit_test_button(point, &btn_idx);
    if (btn.has_value()) {
        res.area = HitArea::Button;
        res.button = btn;
        res.button_index = btn_idx;
        return res;
    }

    // 2. Resize grips (if resizable)
    EdgeGrip grip = hit_test_grip(point);
    if (grip != EdgeGrip::None) {
        res.area = HitArea::ResizeBorder;
        res.edge = grip;
        return res;
    }

    // 3. Caption bar
    if (caption_rect.contains(point)) {
        res.area = HitArea::Caption;
        return res;
    }

    // 4. Client content area
    if (client_rect.contains(point)) {
        res.area = HitArea::Client;
        return res;
    }

    // 5. Visible frame borders
    if (frame_rect.contains(point)) {
        if (!is_maximized(state) && !is_fullscreen(state) && !is_tiled(state)) {
            res.area = HitArea::ResizeBorder;
            // Best effort edge assignment if within frame rect
            if (point.y < client_rect.y) res.edge = EdgeGrip::Top;
            else if (point.y >= client_rect.bottom()) res.edge = EdgeGrip::Bottom;
            else if (point.x < client_rect.x) res.edge = EdgeGrip::Left;
            else if (point.x >= client_rect.right()) res.edge = EdgeGrip::Right;
        } else {
            res.area = HitArea::Caption;
        }
        return res;
    }

    // 6. Drop shadow area
    if (!shadow_rect.empty() && shadow_rect.contains(point)) {
        res.area = HitArea::Shadow;
        return res;
    }

    return res;
}

static void compute_internal_geometry(
    FrameGeometry& geom,
    const FrameMetrics& metrics,
    FrameState state,
    const WindowCaption& caption
) noexcept {
    geom.state = state;
    geom.corner_radius = metrics.effective_corner_radius(state);

    float H = metrics.effective_caption_height(state);
    Margins borders = metrics.effective_borders(state);

    // Caption rect
    if (H > 0.0f) {
        geom.caption_rect = Rect(
            geom.frame_rect.x + borders.left,
            geom.frame_rect.y + borders.top,
            std::max(0.0f, geom.frame_rect.width - borders.horizontal()),
            H
        );
    } else {
        geom.caption_rect = Rect{0.0f, 0.0f, 0.0f, 0.0f};
    }

    // Buttons
    geom.buttons.clear();
    float btn_w = metrics.button_width;
    float btn_h = std::min(H, metrics.button_height);
    float btn_y = geom.caption_rect.y + std::max(0.0f, (H - btn_h) * 0.5f);

    size_t num_buttons = metrics.buttons.size();
    if (H > 0.0f && num_buttons > 0) {
        geom.buttons.reserve(num_buttons);

        if (!metrics.left_handed_buttons) {
            // Right-aligned buttons (Windows/Linux style)
            float right_edge = geom.caption_rect.right() - metrics.button_margin_right;

            for (size_t i = 0; i < num_buttons; ++i) {
                ButtonKind k = metrics.buttons[i];
                if (is_maximized(state) && k == ButtonKind::Maximize) {
                    k = ButtonKind::Restore;
                } else if (!is_maximized(state) && k == ButtonKind::Restore) {
                    k = ButtonKind::Maximize;
                }

                size_t offset_from_right = num_buttons - 1 - i;
                float x = right_edge - static_cast<float>(offset_from_right + 1) * btn_w -
                          static_cast<float>(offset_from_right) * metrics.button_spacing;

                geom.buttons.push_back({k, Rect(x, btn_y, btn_w, btn_h)});
            }
        } else {
            // Left-aligned buttons (macOS style)
            float left_edge = geom.caption_rect.x + metrics.button_margin_left;

            for (size_t i = 0; i < num_buttons; ++i) {
                ButtonKind k = metrics.buttons[i];
                if (is_maximized(state) && k == ButtonKind::Maximize) {
                    k = ButtonKind::Restore;
                } else if (!is_maximized(state) && k == ButtonKind::Restore) {
                    k = ButtonKind::Maximize;
                }

                float x = left_edge + static_cast<float>(i) * (btn_w + metrics.button_spacing);
                geom.buttons.push_back({k, Rect(x, btn_y, btn_w, btn_h)});
            }
        }
    }

    // Title and Icon
    if (H > 0.0f) {
        float avail_left = geom.caption_rect.x + metrics.title_margin_left;
        float avail_right = geom.caption_rect.right() - metrics.title_margin_right;

        if (!metrics.left_handed_buttons && !geom.buttons.empty()) {
            avail_right = geom.buttons.front().rect.x - metrics.button_spacing;
        } else if (metrics.left_handed_buttons && !geom.buttons.empty()) {
            avail_left = geom.buttons.back().rect.right() + metrics.button_spacing;
        }

        float content_x = avail_left;

        if (caption.show_icon && metrics.icon_size > 0.0f) {
            float icon_y = geom.caption_rect.y + (H - metrics.icon_size) * 0.5f;
            geom.icon_rect = Rect(content_x, icon_y, metrics.icon_size, metrics.icon_size);
            content_x += metrics.icon_size + 6.0f;
        } else {
            geom.icon_rect = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        }

        float title_w = std::max(0.0f, avail_right - content_x);
        geom.title_rect = Rect(content_x, geom.caption_rect.y, title_w, H);
    } else {
        geom.icon_rect = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.title_rect = Rect{0.0f, 0.0f, 0.0f, 0.0f};
    }

    // Drop shadow
    if (is_maximized(state) || is_fullscreen(state) || is_tiled(state)) {
        geom.shadow_margins = Margins{0.0f, 0.0f, 0.0f, 0.0f};
        geom.shadow_rect = geom.frame_rect;
    } else {
        geom.shadow_margins = calculate_shadow_margins(metrics.shadow);
        geom.shadow_rect = Rect(
            geom.frame_rect.x - geom.shadow_margins.left,
            geom.frame_rect.y - geom.shadow_margins.top,
            geom.frame_rect.width + geom.shadow_margins.horizontal(),
            geom.frame_rect.height + geom.shadow_margins.vertical()
        );
    }

    // Resize grips
    bool resizable = !is_maximized(state) && !is_fullscreen(state) && !is_tiled(state);
    if (resizable && geom.frame_rect.width > 0.0f && geom.frame_rect.height > 0.0f) {
        float G = metrics.resize_grip_size;
        float L = metrics.corner_grip_length;
        float fw = geom.frame_rect.width;
        float fh = geom.frame_rect.height;
        float fx = geom.frame_rect.x;
        float fy = geom.frame_rect.y;

        float cl_x = std::min(L, fw * 0.5f);
        float cl_y = std::min(L, fh * 0.5f);

        geom.grip_top_left = Rect(fx - G * 0.5f, fy - G * 0.5f, cl_x + G * 0.5f, cl_y + G * 0.5f);
        geom.grip_top_right = Rect(fx + fw - cl_x, fy - G * 0.5f, cl_x + G * 0.5f, cl_y + G * 0.5f);
        geom.grip_bottom_left = Rect(fx - G * 0.5f, fy + fh - cl_y, cl_x + G * 0.5f, cl_y + G * 0.5f);
        geom.grip_bottom_right = Rect(fx + fw - cl_x, fy + fh - cl_y, cl_x + G * 0.5f, cl_y + G * 0.5f);

        geom.grip_top = Rect(fx + cl_x, fy - G * 0.5f, std::max(0.0f, fw - 2.0f * cl_x), G);
        geom.grip_bottom = Rect(fx + cl_x, fy + fh - G * 0.5f, std::max(0.0f, fw - 2.0f * cl_x), G);
        geom.grip_left = Rect(fx - G * 0.5f, fy + cl_y, G, std::max(0.0f, fh - 2.0f * cl_y));
        geom.grip_right = Rect(fx + fw - G * 0.5f, fy + cl_y, G, std::max(0.0f, fh - 2.0f * cl_y));
    } else {
        geom.grip_top_left = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_top_right = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_bottom_left = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_bottom_right = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_top = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_bottom = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_left = Rect{0.0f, 0.0f, 0.0f, 0.0f};
        geom.grip_right = Rect{0.0f, 0.0f, 0.0f, 0.0f};
    }
}

FrameGeometry compute_frame_geometry_from_client(
    const Rect& client_rect,
    const FrameMetrics& metrics,
    FrameState state,
    const WindowCaption& caption
) noexcept {
    Margins borders = metrics.effective_borders(state);
    float H = metrics.effective_caption_height(state);

    FrameGeometry geom;
    geom.client_rect = client_rect;
    geom.border_margins = Margins{borders.left, borders.top + H, borders.right, borders.bottom};

    geom.frame_rect = Rect(
        client_rect.x - borders.left,
        client_rect.y - (borders.top + H),
        client_rect.width + borders.horizontal(),
        client_rect.height + borders.vertical() + H
    );

    compute_internal_geometry(geom, metrics, state, caption);
    return geom;
}

FrameGeometry compute_frame_geometry_from_frame(
    const Rect& frame_rect,
    const FrameMetrics& metrics,
    FrameState state,
    const WindowCaption& caption
) noexcept {
    Margins borders = metrics.effective_borders(state);
    float H = metrics.effective_caption_height(state);

    FrameGeometry geom;
    geom.frame_rect = frame_rect;
    geom.border_margins = Margins{borders.left, borders.top + H, borders.right, borders.bottom};

    geom.client_rect = Rect(
        frame_rect.x + borders.left,
        frame_rect.y + borders.top + H,
        std::max(0.0f, frame_rect.width - borders.horizontal()),
        std::max(0.0f, frame_rect.height - (borders.vertical() + H))
    );

    compute_internal_geometry(geom, metrics, state, caption);
    return geom;
}

} // namespace brodecor
