#pragma once

#include "brodecor/shadow.h"
#include "brodecor/style.h"
#include "brodecor/types.h"

#include <functional>
#include <optional>
#include <vector>

namespace brodecor {

// --- Frame Metrics ---

struct FrameMetrics {
    float caption_height = 32.0f;
    float border_thickness = 4.0f;
    float resize_grip_size = 8.0f;
    float corner_radius = 8.0f;
    float corner_grip_length = 24.0f;

    float button_width = 36.0f;
    float button_height = 24.0f;
    float button_spacing = 2.0f;
    float button_margin_right = 6.0f;
    float button_margin_left = 6.0f;

    float title_margin_left = 10.0f;
    float title_margin_right = 10.0f;
    float icon_size = 16.0f;
    float icon_margin_left = 10.0f;

    ShadowParams shadow;

    bool left_handed_buttons = false;
    std::vector<ButtonKind> buttons = {
        ButtonKind::Minimize,
        ButtonKind::Maximize,
        ButtonKind::Close
    };

    Margins effective_borders(FrameState state) const noexcept;
    float effective_caption_height(FrameState state) const noexcept;
    float effective_corner_radius(FrameState state) const noexcept;
};

// --- Hit Testing ---

enum class HitArea : uint8_t {
    None = 0,
    Client,
    Caption,
    Button,
    ResizeBorder,
    Shadow
};

inline const char* to_string(HitArea area) noexcept {
    switch (area) {
        case HitArea::None: return "None";
        case HitArea::Client: return "Client";
        case HitArea::Caption: return "Caption";
        case HitArea::Button: return "Button";
        case HitArea::ResizeBorder: return "ResizeBorder";
        case HitArea::Shadow: return "Shadow";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, HitArea area) {
    return os << to_string(area);
}

struct HitTestResult {
    HitArea area = HitArea::None;
    EdgeGrip edge = EdgeGrip::None;
    std::optional<ButtonKind> button = std::nullopt;
    int button_index = -1;

    constexpr bool is_client() const noexcept { return area == HitArea::Client; }
    constexpr bool is_caption() const noexcept { return area == HitArea::Caption; }
    constexpr bool is_button() const noexcept { return area == HitArea::Button; }
    constexpr bool is_resizable() const noexcept {
        return area == HitArea::ResizeBorder && edge != EdgeGrip::None;
    }
    constexpr bool is_draggable() const noexcept { return area == HitArea::Caption; }
    constexpr bool is_shadow() const noexcept { return area == HitArea::Shadow; }

    constexpr bool operator==(const HitTestResult& o) const noexcept = default;
};

// --- Computed Frame Geometry ---

struct FrameGeometry {
    Rect frame_rect;
    Rect client_rect;
    Rect caption_rect;
    Rect title_rect;
    Rect icon_rect;

    struct ButtonItem {
        ButtonKind kind;
        Rect rect;

        constexpr bool operator==(const ButtonItem& o) const noexcept = default;
    };
    std::vector<ButtonItem> buttons;

    Rect shadow_rect;
    Margins shadow_margins;
    Margins border_margins;
    float corner_radius = 0.0f;
    FrameState state = FrameState::Active;

    // Edge grip hitboxes
    Rect grip_top;
    Rect grip_bottom;
    Rect grip_left;
    Rect grip_right;
    Rect grip_top_left;
    Rect grip_top_right;
    Rect grip_bottom_left;
    Rect grip_bottom_right;

    Rect grip_rect(EdgeGrip grip) const noexcept;
    EdgeGrip hit_test_grip(Vec2 point) const noexcept;
    std::optional<ButtonKind> hit_test_button(Vec2 point, int* out_index = nullptr) const noexcept;
    HitTestResult hit_test(Vec2 point) const noexcept;
};

FrameGeometry compute_frame_geometry_from_client(
    const Rect& client_rect,
    const FrameMetrics& metrics,
    FrameState state,
    const WindowCaption& caption = {}
) noexcept;

FrameGeometry compute_frame_geometry_from_frame(
    const Rect& frame_rect,
    const FrameMetrics& metrics,
    FrameState state,
    const WindowCaption& caption = {}
) noexcept;

// --- Button State & Animations ---

struct ButtonStateInfo {
    ButtonKind kind;
    ButtonState state = ButtonState::Normal;
    float hover_progress = 0.0f;
    float press_progress = 0.0f;
    bool is_disabled = false;
};

class ButtonStateManager {
public:
    ButtonStateManager() = default;
    explicit ButtonStateManager(const std::vector<ButtonKind>& kinds);

    void set_buttons(const std::vector<ButtonKind>& kinds);
    void reset();

    bool on_pointer_move(Vec2 point, const FrameGeometry& geom) noexcept;
    bool on_pointer_down(Vec2 point, const FrameGeometry& geom, int button = 1) noexcept;
    bool on_pointer_up(
        Vec2 point,
        const FrameGeometry& geom,
        int button = 1,
        const std::function<void(ButtonKind)>& on_click = nullptr
    ) noexcept;
    void on_pointer_leave() noexcept;

    void tick(float dt_seconds) noexcept;

    ButtonState get_state(ButtonKind kind) const noexcept;
    float get_hover_progress(ButtonKind kind) const noexcept;
    float get_press_progress(ButtonKind kind) const noexcept;

    void set_disabled(ButtonKind kind, bool disabled) noexcept;
    bool is_disabled(ButtonKind kind) const noexcept;

    const std::vector<ButtonStateInfo>& all_states() const noexcept { return items_; }

private:
    std::vector<ButtonStateInfo> items_;
    std::optional<ButtonKind> pressed_button_ = std::nullopt;
    float animation_speed_ = 10.0f; // 1/s rate

    ButtonStateInfo* find_item(ButtonKind kind) noexcept;
    const ButtonStateInfo* find_item(ButtonKind kind) const noexcept;
};

// --- Decorator Engine ---

class DecoratorEngine {
public:
    DecoratorEngine();
    explicit DecoratorEngine(const FrameMetrics& metrics, const FrameStyle& style = FrameStyle::dark());

    // Window configuration
    void set_client_rect(const Rect& client_rect) noexcept;
    void set_frame_rect(const Rect& frame_rect) noexcept;
    void set_state(FrameState state) noexcept;
    void set_metrics(const FrameMetrics& metrics) noexcept;
    void set_style(const FrameStyle& style) noexcept;
    void set_caption(const WindowCaption& caption) noexcept;
    void set_title(std::string_view title);

    // Queries
    const FrameGeometry& geometry() const noexcept { return geom_; }
    const FrameMetrics& metrics() const noexcept { return metrics_; }
    const FrameStyle& style() const noexcept { return style_; }
    const WindowCaption& caption() const noexcept { return caption_; }
    FrameState state() const noexcept { return state_; }

    ButtonState button_state(ButtonKind kind) const noexcept {
        return buttons_.get_state(kind);
    }
    float button_hover_progress(ButtonKind kind) const noexcept {
        return buttons_.get_hover_progress(kind);
    }
    float button_press_progress(ButtonKind kind) const noexcept {
        return buttons_.get_press_progress(kind);
    }
    void set_button_disabled(ButtonKind kind, bool disabled) noexcept {
        buttons_.set_disabled(kind, disabled);
    }

    // Input events
    HitTestResult hit_test(Vec2 point) const noexcept;
    bool on_pointer_move(Vec2 point) noexcept;
    bool on_pointer_down(Vec2 point, int button = 1) noexcept;
    bool on_pointer_up(Vec2 point, int button = 1) noexcept;
    void on_pointer_leave() noexcept;

    // Tick animations
    void tick(float dt_seconds) noexcept;

    // Shadow mesh generation
    ShadowMesh generate_shadow_mesh(bool include_center = true) const noexcept;

    // Callbacks
    std::function<void(ButtonKind)> on_button_click;
    std::function<void(FrameState)> on_state_changed;

private:
    FrameMetrics metrics_;
    FrameStyle style_;
    WindowCaption caption_;
    FrameState state_ = FrameState::Active;
    FrameGeometry geom_;
    ButtonStateManager buttons_;
    bool defined_by_client_ = true;

    void update_geometry() noexcept;
};

} // namespace brodecor
