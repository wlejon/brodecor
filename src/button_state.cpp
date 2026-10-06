#include "brodecor/decor.h"

#include <algorithm>

namespace brodecor {

ButtonStateManager::ButtonStateManager(const std::vector<ButtonKind>& kinds) {
    set_buttons(kinds);
}

void ButtonStateManager::set_buttons(const std::vector<ButtonKind>& kinds) {
    std::vector<ButtonStateInfo> new_items;
    new_items.reserve(kinds.size());

    for (ButtonKind k : kinds) {
        const ButtonStateInfo* existing = find_item(k);
        if (existing) {
            new_items.push_back(*existing);
        } else {
            new_items.push_back(ButtonStateInfo{
                k,
                ButtonState::Normal,
                0.0f,
                0.0f,
                false
            });
        }
    }
    items_ = std::move(new_items);

    if (pressed_button_.has_value() && !find_item(*pressed_button_)) {
        pressed_button_ = std::nullopt;
    }
}

void ButtonStateManager::reset() {
    pressed_button_ = std::nullopt;
    for (auto& item : items_) {
        if (!item.is_disabled) {
            item.state = ButtonState::Normal;
        }
        item.hover_progress = 0.0f;
        item.press_progress = 0.0f;
    }
}

ButtonStateInfo* ButtonStateManager::find_item(ButtonKind kind) noexcept {
    for (auto& item : items_) {
        if (item.kind == kind) return &item;
    }
    return nullptr;
}

const ButtonStateInfo* ButtonStateManager::find_item(ButtonKind kind) const noexcept {
    for (const auto& item : items_) {
        if (item.kind == kind) return &item;
    }
    return nullptr;
}

bool ButtonStateManager::on_pointer_move(Vec2 point, const FrameGeometry& geom) noexcept {
    if (pressed_button_.has_value()) {
        ButtonStateInfo* pressed_item = find_item(*pressed_button_);
        if (pressed_item) {
            auto hit = geom.hit_test_button(point);
            if (hit.has_value() && *hit == *pressed_button_) {
                pressed_item->state = ButtonState::Pressed;
            } else {
                pressed_item->state = ButtonState::Hover;
            }
        }
        return true;
    }

    auto hit = geom.hit_test_button(point);
    bool any_hovered = false;

    for (auto& item : items_) {
        if (item.is_disabled) {
            item.state = ButtonState::Disabled;
            continue;
        }

        if (hit.has_value() && *hit == item.kind) {
            item.state = ButtonState::Hover;
            any_hovered = true;
        } else {
            item.state = ButtonState::Normal;
        }
    }

    return any_hovered;
}

bool ButtonStateManager::on_pointer_down(Vec2 point, const FrameGeometry& geom, int button) noexcept {
    if (button != 1) {
        return false;
    }

    auto hit = geom.hit_test_button(point);
    if (!hit.has_value()) {
        return false;
    }

    ButtonStateInfo* item = find_item(*hit);
    if (item && !item->is_disabled) {
        item->state = ButtonState::Pressed;
        pressed_button_ = *hit;
        return true;
    }

    return false;
}

bool ButtonStateManager::on_pointer_up(
    Vec2 point,
    const FrameGeometry& geom,
    int button,
    const std::function<void(ButtonKind)>& on_click
) noexcept {
    if (button != 1 || !pressed_button_.has_value()) {
        return false;
    }

    ButtonKind clicked_kind = *pressed_button_;
    pressed_button_ = std::nullopt;

    ButtonStateInfo* item = find_item(clicked_kind);
    auto hit = geom.hit_test_button(point);

    if (hit.has_value() && *hit == clicked_kind && item && !item->is_disabled) {
        item->state = ButtonState::Hover;
        if (on_click) {
            on_click(clicked_kind);
        }
        return true;
    }

    if (item && !item->is_disabled) {
        item->state = ButtonState::Normal;
        return true;
    }

    return false;
}

void ButtonStateManager::on_pointer_leave() noexcept {
    if (!pressed_button_.has_value()) {
        for (auto& item : items_) {
            if (!item.is_disabled) {
                item.state = ButtonState::Normal;
            }
        }
    }
}

void ButtonStateManager::tick(float dt_seconds) noexcept {
    float step = std::clamp(dt_seconds * animation_speed_, 0.0f, 1.0f);

    for (auto& item : items_) {
        float target_hover = (item.state == ButtonState::Hover || item.state == ButtonState::Pressed) ? 1.0f : 0.0f;
        float target_press = (item.state == ButtonState::Pressed) ? 1.0f : 0.0f;

        item.hover_progress = bromath::lerp(item.hover_progress, target_hover, step);
        item.press_progress = bromath::lerp(item.press_progress, target_press, step);
    }
}

ButtonState ButtonStateManager::get_state(ButtonKind kind) const noexcept {
    const ButtonStateInfo* item = find_item(kind);
    return item ? item->state : ButtonState::Normal;
}

float ButtonStateManager::get_hover_progress(ButtonKind kind) const noexcept {
    const ButtonStateInfo* item = find_item(kind);
    return item ? item->hover_progress : 0.0f;
}

float ButtonStateManager::get_press_progress(ButtonKind kind) const noexcept {
    const ButtonStateInfo* item = find_item(kind);
    return item ? item->press_progress : 0.0f;
}

void ButtonStateManager::set_disabled(ButtonKind kind, bool disabled) noexcept {
    ButtonStateInfo* item = find_item(kind);
    if (item) {
        item->is_disabled = disabled;
        if (disabled) {
            item->state = ButtonState::Disabled;
            if (pressed_button_ == kind) {
                pressed_button_ = std::nullopt;
            }
        } else if (item->state == ButtonState::Disabled) {
            item->state = ButtonState::Normal;
        }
    }
}

bool ButtonStateManager::is_disabled(ButtonKind kind) const noexcept {
    const ButtonStateInfo* item = find_item(kind);
    return item ? item->is_disabled : false;
}

} // namespace brodecor
