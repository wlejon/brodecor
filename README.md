# brodecor

Standalone, reusable C++20 window frame, titlebar, button, drop-shadow, and resize-border engine for Server-Side Window Decorations (SSD) and XWayland in the **bro** ecosystem.

## Overview

`brodecor` provides a pure, portable decoration engine for Wayland compositors (such as `brocompositor`), XWayland window tracking, and desktop window management. It separates window geometry layout, hit testing, button state machines, style theming, and drop-shadow quad mesh generation into modular, testable components.

## Architecture

- **`include/brodecor/types.h`**:
  - `Rect` and `Margins`: 2D geometry primitives interoperating with `bromath::Vec2` and `bromath::AABB2`.
  - `ButtonKind`: `Close`, `Minimize`, `Maximize`, `Restore`, `Menu`.
  - `ButtonState`: `Normal`, `Hover`, `Pressed`, `Disabled`.
  - `EdgeGrip`: `Top`, `Bottom`, `Left`, `Right`, `TopLeft`, `TopRight`, `BottomLeft`, `BottomRight`.
  - `FrameState`: Bitflag enum supporting `Normal`, `Active`, `Inactive`, `Maximized`, `Fullscreen`, and `Tiled`.
  - `WindowCaption`: Window title, subtitle, icon, and alignment configuration.

- **`include/brodecor/decor.h`**:
  - `FrameMetrics`: Titlebar height, border thickness, resize grip hit-test thickness, corner radius, button dimensions and layout order.
  - `HitArea` & `HitTestResult`: Classification of pointer hit tests (`Client`, `Caption`, `Button`, `ResizeBorder`, `Shadow`).
  - `FrameGeometry`: Computed layout rects for frame, client area, caption, buttons, title text, icon, shadow, and 8-direction edge/corner grip hitboxes.
  - `ButtonStateManager`: State transitions (hover, press, drag-out cancel, release click) and smooth animation interpolation.
  - `DecoratorEngine`: High-level controller coordinating metrics, style, caption, state, input routing, and rendering data.

- **`include/brodecor/shadow.h` & `src/shadow_mesh.cpp`**:
  - `ShadowParams`: Blur size, directional offset, spread, corner radius, and color.
  - Procedural 9-slice blur shadow quad generator:
    - Produces a 4x4 grid (16 vertices, 54 indices for full 9-slice, 48 indices for center-hollowed 8-slice).
    - Analytical vertex alpha falloff from solid inner bounds to transparent outer edges.
    - Normalized UVs for texture or shader mapping.

- **`include/brodecor/style.h` & `src/style.cpp`**:
  - `FrameColors` and `ButtonStyle`: State-specific background, foreground, border, and accent colors.
  - Built-in theme presets: `dark()`, `light()`, and `high_contrast()`.
  - Smooth color interpolation across active/inactive frame transitions.

## Dependencies

- **[`bromath`](https://github.com/wlejon/bromath)**: Vector (`Vec2`), color (`Color`, `Color8`), and bounding box math.
- **[bronze](https://github.com/wlejon/bronze)** (with brass, its code generator) for the JavaScript binding.
- No external third-party dependencies.

A plain `git clone` is enough: there are no submodules. Each dependency is a `bro_dependency()`
call in `CMakeLists.txt` (`cmake/bro_deps.cmake`): an existing target wins, then a working tree at
`../<name>`, then the head of its main branch, fetched at configure. Override one with
`-DFETCHCONTENT_SOURCE_DIR_<NAME>=<path>`.

## Building and Testing

```bash
mkdir -p build && cd build
cmake -G Ninja ..
ninja
ctest --output-on-failure
```
