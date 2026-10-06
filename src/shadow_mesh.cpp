#include "brodecor/shadow.h"

#include <algorithm>

namespace brodecor {

Margins calculate_shadow_margins(const ShadowParams& params) noexcept {
    if (params.size <= 0.0f && params.spread <= 0.0f) {
        return Margins{0.0f, 0.0f, 0.0f, 0.0f};
    }

    float l = std::max(0.0f, params.size - params.offset.x + params.spread);
    float r = std::max(0.0f, params.size + params.offset.x + params.spread);
    float t = std::max(0.0f, params.size - params.offset.y + params.spread);
    float b = std::max(0.0f, params.size + params.offset.y + params.spread);

    return Margins{l, t, r, b};
}

ShadowMesh generate_shadow_mesh(
    const Rect& frame_rect,
    const ShadowParams& params,
    bool include_center
) noexcept {
    if (frame_rect.empty() || (params.size <= 0.0f && params.spread <= 0.0f) || params.color.a <= 0.0f) {
        return ShadowMesh{};
    }

    Margins margins = calculate_shadow_margins(params);
    if (margins.empty()) {
        return ShadowMesh{};
    }

    float outer_x = frame_rect.x - margins.left;
    float outer_y = frame_rect.y - margins.top;
    float outer_w = frame_rect.width + margins.horizontal();
    float outer_h = frame_rect.height + margins.vertical();

    if (outer_w <= 0.0f || outer_h <= 0.0f) {
        return ShadowMesh{};
    }

    // Coordinates of 4 grid lines along X and Y
    float x0 = outer_x;
    float x1 = frame_rect.x + params.offset.x - params.spread;
    float x2 = frame_rect.right() + params.offset.x + params.spread;
    float x3 = outer_x + outer_w;

    // Enforce proper monotonicity
    if (x1 > x2) {
        float mid = (x1 + x2) * 0.5f;
        x1 = mid;
        x2 = mid;
    }
    x1 = std::clamp(x1, x0, x3);
    x2 = std::clamp(x2, x1, x3);

    float y0 = outer_y;
    float y1 = frame_rect.y + params.offset.y - params.spread;
    float y2 = frame_rect.bottom() + params.offset.y + params.spread;
    float y3 = outer_y + outer_h;

    if (y1 > y2) {
        float mid = (y1 + y2) * 0.5f;
        y1 = mid;
        y2 = mid;
    }
    y1 = std::clamp(y1, y0, y3);
    y2 = std::clamp(y2, y1, y3);

    float xs[4] = {x0, x1, x2, x3};
    float ys[4] = {y0, y1, y2, y3};

    ShadowMesh mesh;
    mesh.bounds = Rect(outer_x, outer_y, outer_w, outer_h);
    mesh.margins = margins;
    mesh.vertices.reserve(16);

    Color transparent = Color(params.color.r, params.color.g, params.color.b, 0.0f);
    Color solid = params.color;

    // Build 4x4 = 16 vertices
    for (int r = 0; r < 4; ++r) {
        float py = ys[r];
        float v = (outer_h > 0.0f) ? ((py - outer_y) / outer_h) : 0.0f;

        for (int c = 0; c < 4; ++c) {
            float px = xs[c];
            float u = (outer_w > 0.0f) ? ((px - outer_x) / outer_w) : 0.0f;

            // Outer ring has 0 alpha; inner 2x2 has full shadow color
            bool is_inner = (r >= 1 && r <= 2 && c >= 1 && c <= 2);
            Color vcol = is_inner ? solid : transparent;

            mesh.vertices.push_back(ShadowVertex{
                Vec2{px, py},
                Vec2{u, v},
                vcol
            });
        }
    }

    // Build 9 quads (each quad = 2 triangles = 6 indices)
    size_t expected_quads = include_center ? 9 : 8;
    mesh.indices.reserve(expected_quads * 6);

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (!include_center && r == 1 && c == 1) {
                continue;
            }

            uint16_t v00 = static_cast<uint16_t>(r * 4 + c);
            uint16_t v01 = static_cast<uint16_t>(r * 4 + (c + 1));
            uint16_t v11 = static_cast<uint16_t>((r + 1) * 4 + (c + 1));
            uint16_t v10 = static_cast<uint16_t>((r + 1) * 4 + c);

            mesh.indices.push_back(v00);
            mesh.indices.push_back(v01);
            mesh.indices.push_back(v11);

            mesh.indices.push_back(v00);
            mesh.indices.push_back(v11);
            mesh.indices.push_back(v10);
        }
    }

    return mesh;
}

} // namespace brodecor
