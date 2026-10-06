#include "check.h"
#include <brodecor/shadow.h>

using namespace brodecor;

int main() {
    // 1. Margin calculations
    {
        // Symmetric
        ShadowParams p_sym;
        p_sym.size = 20.0f;
        p_sym.offset = {0.0f, 0.0f};
        p_sym.spread = 2.0f;

        Margins m_sym = calculate_shadow_margins(p_sym);
        CHECK_NEAR(m_sym.left, 22.0f, 1e-4);
        CHECK_NEAR(m_sym.right, 22.0f, 1e-4);
        CHECK_NEAR(m_sym.top, 22.0f, 1e-4);
        CHECK_NEAR(m_sym.bottom, 22.0f, 1e-4);
        CHECK_NEAR(m_sym.horizontal(), 44.0f, 1e-4);
        CHECK_NEAR(m_sym.vertical(), 44.0f, 1e-4);

        // Asymmetric
        ShadowParams p_asym;
        p_asym.size = 20.0f;
        p_asym.offset = {4.0f, 6.0f};
        p_asym.spread = 0.0f;

        Margins m_asym = calculate_shadow_margins(p_asym);
        CHECK_NEAR(m_asym.left, 16.0f, 1e-4);   // 20 - 4
        CHECK_NEAR(m_asym.right, 24.0f, 1e-4);  // 20 + 4
        CHECK_NEAR(m_asym.top, 14.0f, 1e-4);    // 20 - 6
        CHECK_NEAR(m_asym.bottom, 26.0f, 1e-4); // 20 + 6

        // Zero shadow
        ShadowParams p_zero;
        p_zero.size = 0.0f;
        p_zero.spread = 0.0f;
        Margins m_zero = calculate_shadow_margins(p_zero);
        CHECK_TRUE(m_zero.empty());
    }

    // 2. 9-slice mesh generation (with center)
    {
        Rect frame{100.0f, 100.0f, 600.0f, 400.0f};
        ShadowParams p;
        p.size = 24.0f;
        p.offset = {0.0f, 8.0f};
        p.spread = 0.0f;
        p.color = Color(0.0f, 0.0f, 0.0f, 0.5f);

        ShadowMesh mesh = generate_shadow_mesh(frame, p, true);

        CHECK_FALSE(mesh.empty());
        CHECK_EQ(mesh.vertices.size(), 16u);
        CHECK_EQ(mesh.indices.size(), 54u); // 9 quads * 6 = 54
        CHECK_EQ(mesh.quad_count(), 9u);

        // Bounds check
        Margins m = calculate_shadow_margins(p);
        CHECK_NEAR(mesh.bounds.x, frame.x - m.left, 1e-4);
        CHECK_NEAR(mesh.bounds.y, frame.y - m.top, 1e-4);
        CHECK_NEAR(mesh.bounds.width, frame.width + m.horizontal(), 1e-4);
        CHECK_NEAR(mesh.bounds.height, frame.height + m.vertical(), 1e-4);

        // Check outer ring alpha (must be 0)
        // Row 0 vertices: index 0, 1, 2, 3
        for (int c = 0; c < 4; ++c) {
            CHECK_NEAR(mesh.vertices[c].color.a, 0.0f, 1e-4);
        }
        // Row 3 vertices: index 12, 13, 14, 15
        for (int c = 0; c < 4; ++c) {
            CHECK_NEAR(mesh.vertices[12 + c].color.a, 0.0f, 1e-4);
        }
        // Col 0 vertices: 0, 4, 8, 12
        for (int r = 0; r < 4; ++r) {
            CHECK_NEAR(mesh.vertices[r * 4].color.a, 0.0f, 1e-4);
        }
        // Col 3 vertices: 3, 7, 11, 15
        for (int r = 0; r < 4; ++r) {
            CHECK_NEAR(mesh.vertices[r * 4 + 3].color.a, 0.0f, 1e-4);
        }

        // Check inner 2x2 vertices (must have full alpha = 0.5)
        // (r=1, c=1) -> 5
        // (r=1, c=2) -> 6
        // (r=2, c=1) -> 9
        // (r=2, c=2) -> 10
        CHECK_NEAR(mesh.vertices[5].color.a, 0.5f, 1e-4);
        CHECK_NEAR(mesh.vertices[6].color.a, 0.5f, 1e-4);
        CHECK_NEAR(mesh.vertices[9].color.a, 0.5f, 1e-4);
        CHECK_NEAR(mesh.vertices[10].color.a, 0.5f, 1e-4);

        // UV range check [0, 1]
        for (const auto& v : mesh.vertices) {
            CHECK_TRUE(v.uv.x >= 0.0f && v.uv.x <= 1.0f);
            CHECK_TRUE(v.uv.y >= 0.0f && v.uv.y <= 1.0f);
        }
    }

    // 3. 8-slice mesh generation (without center)
    {
        Rect frame{100.0f, 100.0f, 400.0f, 300.0f};
        ShadowParams p;
        p.size = 16.0f;
        p.color = Color(0.1f, 0.1f, 0.1f, 0.4f);

        ShadowMesh mesh = generate_shadow_mesh(frame, p, false);

        CHECK_FALSE(mesh.empty());
        CHECK_EQ(mesh.vertices.size(), 16u);
        CHECK_EQ(mesh.indices.size(), 48u); // 8 quads * 6 = 48
        CHECK_EQ(mesh.quad_count(), 8u);
    }

    // 4. Degenerate and invisible cases
    {
        ShadowParams p;
        p.size = 20.0f;

        // Empty frame rect
        ShadowMesh m1 = generate_shadow_mesh(Rect{0, 0, 0, 0}, p);
        CHECK_TRUE(m1.empty());

        // Zero size and spread
        p.size = 0.0f;
        p.spread = 0.0f;
        ShadowMesh m2 = generate_shadow_mesh(Rect{10, 10, 100, 100}, p);
        CHECK_TRUE(m2.empty());

        // Transparent color
        p.size = 20.0f;
        p.color.a = 0.0f;
        ShadowMesh m3 = generate_shadow_mesh(Rect{10, 10, 100, 100}, p);
        CHECK_TRUE(m3.empty());
    }

    return ::brotest::finish("test_shadow_mesh");
}
