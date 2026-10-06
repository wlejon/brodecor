#pragma once

#include "brodecor/types.h"

#include <vector>

namespace brodecor {

struct ShadowParams {
    float size = 24.0f;              // Blur radius / shadow extent
    Vec2 offset = {0.0f, 6.0f};      // Directional drop-shadow offset
    float spread = 0.0f;             // Expansion beyond frame
    float corner_radius = 12.0f;     // Corner rounding
    Color color = Color(0.0f, 0.0f, 0.0f, 0.35f);

    constexpr bool operator==(const ShadowParams& o) const noexcept = default;
};

struct ShadowVertex {
    Vec2 pos;
    Vec2 uv;
    Color color;

    constexpr bool operator==(const ShadowVertex& o) const noexcept = default;
};

struct ShadowMesh {
    std::vector<ShadowVertex> vertices;
    std::vector<uint16_t> indices;
    Rect bounds;
    Margins margins;

    bool empty() const noexcept { return vertices.empty() || indices.empty(); }
    size_t quad_count() const noexcept { return indices.size() / 6; }
};

Margins calculate_shadow_margins(const ShadowParams& params) noexcept;

ShadowMesh generate_shadow_mesh(
    const Rect& frame_rect,
    const ShadowParams& params,
    bool include_center = true
) noexcept;

} // namespace brodecor
