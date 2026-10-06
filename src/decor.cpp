#include "brodecor/decor.h"

namespace brodecor {

DecoratorEngine::DecoratorEngine()
    : DecoratorEngine(FrameMetrics{}, FrameStyle::dark()) {}

DecoratorEngine::DecoratorEngine(const FrameMetrics& metrics, const FrameStyle& style)
    : metrics_(metrics), style_(style), buttons_(metrics.buttons) {
    // Default 800x600 window
    set_client_rect(Rect{100.0f, 100.0f, 800.0f, 600.0f});
}

void DecoratorEngine::set_client_rect(const Rect& client_rect) noexcept {
    defined_by_client_ = true;
    geom_ = compute_frame_geometry_from_client(client_rect, metrics_, state_, caption_);
}

void DecoratorEngine::set_frame_rect(const Rect& frame_rect) noexcept {
    defined_by_client_ = false;
    geom_ = compute_frame_geometry_from_frame(frame_rect, metrics_, state_, caption_);
}

void DecoratorEngine::set_state(FrameState state) noexcept {
    if (state_ != state) {
        state_ = state;
        update_geometry();
        if (on_state_changed) {
            on_state_changed(state_);
        }
    }
}

void DecoratorEngine::set_metrics(const FrameMetrics& metrics) noexcept {
    metrics_ = metrics;
    buttons_.set_buttons(metrics_.buttons);
    update_geometry();
}

void DecoratorEngine::set_style(const FrameStyle& style) noexcept {
    style_ = style;
}

void DecoratorEngine::set_caption(const WindowCaption& caption) noexcept {
    caption_ = caption;
    update_geometry();
}

void DecoratorEngine::set_title(std::string_view title) {
    caption_.title = std::string(title);
    update_geometry();
}

void DecoratorEngine::update_geometry() noexcept {
    if (defined_by_client_) {
        geom_ = compute_frame_geometry_from_client(geom_.client_rect, metrics_, state_, caption_);
    } else {
        geom_ = compute_frame_geometry_from_frame(geom_.frame_rect, metrics_, state_, caption_);
    }
}

HitTestResult DecoratorEngine::hit_test(Vec2 point) const noexcept {
    return geom_.hit_test(point);
}

bool DecoratorEngine::on_pointer_move(Vec2 point) noexcept {
    return buttons_.on_pointer_move(point, geom_);
}

bool DecoratorEngine::on_pointer_down(Vec2 point, int button) noexcept {
    return buttons_.on_pointer_down(point, geom_, button);
}

bool DecoratorEngine::on_pointer_up(Vec2 point, int button) noexcept {
    return buttons_.on_pointer_up(point, geom_, button, on_button_click);
}

void DecoratorEngine::on_pointer_leave() noexcept {
    buttons_.on_pointer_leave();
}

void DecoratorEngine::tick(float dt_seconds) noexcept {
    buttons_.tick(dt_seconds);
}

ShadowMesh DecoratorEngine::generate_shadow_mesh(bool include_center) const noexcept {
    if (is_maximized(state_) || is_fullscreen(state_) || is_tiled(state_)) {
        return ShadowMesh{};
    }
    return brodecor::generate_shadow_mesh(geom_.frame_rect, metrics_.shadow, include_center);
}

} // namespace brodecor
