#include "api.h"
#include "object_builder.h"

#include <string>
#include <vector>

namespace brodecor::api {

namespace {

Value rectToJs(const Rect& r) {
    ObjectBuilder b;
    b.set("x", static_cast<double>(r.x));
    b.set("y", static_cast<double>(r.y));
    b.set("width", static_cast<double>(r.width));
    b.set("height", static_cast<double>(r.height));
    return b.build();
}

Rect rectFromJs(Value v) {
    if (!ev::isObject(v)) return Rect{};
    ev::Persistent vp(v);
    float x = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "x")));
    float y = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "y")));
    float w = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "width")));
    float h = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "height")));
    return Rect{x, y, w, h};
}

Vec2 pointFromJs(Value v) {
    if (!ev::isObject(v)) return Vec2{};
    ev::Persistent vp(v);
    float x = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "x")));
    float y = static_cast<float>(ev::toDouble(ev::getProperty(vp.get(), "y")));
    return Vec2{x, y};
}

FrameState frameStateFromString(std::string_view s) {
    if (s == "maximized" || s == "Maximized") return FrameState::Maximized;
    if (s == "fullscreen" || s == "Fullscreen") return FrameState::Fullscreen;
    if (s == "tiled" || s == "tiled_left" || s == "tiled_right" || s == "Tiled") return FrameState::Tiled;
    if (s == "inactive" || s == "Inactive") return FrameState::Inactive;
    if (s == "normal" || s == "Normal") return FrameState::Normal;
    return FrameState::Active;
}

} // namespace

void installDecorOnto(Value decorVal) {
    ObjectBuilder decor(decorVal);

    // bro.decor.getDefaultMetrics() -> { captionHeight, borderThickness, resizeGripSize, cornerRadius, buttonWidth, buttonHeight }
    decor.def("getDefaultMetrics", 0, [](Value, std::span<const Value>) -> Value {
        FrameMetrics m;
        ObjectBuilder b;
        b.set("captionHeight", static_cast<double>(m.caption_height));
        b.set("borderThickness", static_cast<double>(m.border_thickness));
        b.set("resizeGripSize", static_cast<double>(m.resize_grip_size));
        b.set("cornerRadius", static_cast<double>(m.corner_radius));
        b.set("cornerGripLength", static_cast<double>(m.corner_grip_length));
        b.set("buttonWidth", static_cast<double>(m.button_width));
        b.set("buttonHeight", static_cast<double>(m.button_height));
        b.set("buttonSpacing", static_cast<double>(m.button_spacing));
        b.set("shadowRadius", static_cast<double>(m.shadow.size));
        return b.build();
    });

    // bro.decor.computeGeometry(options)
    decor.def("computeGeometry", 1, [](Value, std::span<const Value> args) -> Value {
        if (args.empty() || !ev::isObject(args[0])) return ev::null();
        ev::Persistent opts(args[0]);

        Rect clientRect = rectFromJs(ev::getProperty(opts.get(), "clientRect"));
        if (clientRect.width <= 0) clientRect.width = 800.0f;
        if (clientRect.height <= 0) clientRect.height = 600.0f;

        FrameMetrics metrics;
        ev::Persistent mVal(ev::getProperty(opts.get(), "metrics"));
        if (ev::isObject(mVal.get())) {
            Value ch = ev::getProperty(mVal.get(), "captionHeight");
            if (ev::isNumber(ch)) metrics.caption_height = static_cast<float>(ev::toDouble(ch));
            Value bt = ev::getProperty(mVal.get(), "borderThickness");
            if (ev::isNumber(bt)) metrics.border_thickness = static_cast<float>(ev::toDouble(bt));
            Value rg = ev::getProperty(mVal.get(), "resizeGripSize");
            if (ev::isNumber(rg)) metrics.resize_grip_size = static_cast<float>(ev::toDouble(rg));
            Value cr = ev::getProperty(mVal.get(), "cornerRadius");
            if (ev::isNumber(cr)) metrics.corner_radius = static_cast<float>(ev::toDouble(cr));
            Value cgl = ev::getProperty(mVal.get(), "cornerGripLength");
            if (ev::isNumber(cgl)) metrics.corner_grip_length = static_cast<float>(ev::toDouble(cgl));
        }

        FrameState state = FrameState::Active;
        Value sVal = ev::getProperty(opts.get(), "state");
        if (ev::isString(sVal)) {
            state = frameStateFromString(ev::toUtf8(sVal));
        }

        WindowCaption caption;
        Value tVal = ev::getProperty(opts.get(), "title");
        if (ev::isString(tVal)) {
            caption.title = ev::toUtf8(tVal);
        }

        FrameGeometry geom = compute_frame_geometry_from_client(clientRect, metrics, state, caption);

        ObjectBuilder res;
        res.set("frameRect", rectToJs(geom.frame_rect));
        res.set("clientRect", rectToJs(geom.client_rect));
        res.set("captionRect", rectToJs(geom.caption_rect));
        res.set("titleRect", rectToJs(geom.title_rect));
        res.set("iconRect", rectToJs(geom.icon_rect));
        res.set("shadowRect", rectToJs(geom.shadow_rect));
        res.set("state", std::string(to_string(geom.state)));

        ObjectBuilder grips;
        grips.set("top", rectToJs(geom.grip_top));
        grips.set("bottom", rectToJs(geom.grip_bottom));
        grips.set("left", rectToJs(geom.grip_left));
        grips.set("right", rectToJs(geom.grip_right));
        grips.set("topLeft", rectToJs(geom.grip_top_left));
        grips.set("topRight", rectToJs(geom.grip_top_right));
        grips.set("bottomLeft", rectToJs(geom.grip_bottom_left));
        grips.set("bottomRight", rectToJs(geom.grip_bottom_right));
        res.set("grips", grips.build());

        ev::Persistent buttonsArr(ev::makeArray(static_cast<uint32_t>(geom.buttons.size())));
        for (size_t i = 0; i < geom.buttons.size(); ++i) {
            ObjectBuilder btn;
            btn.set("kind", std::string(to_string(geom.buttons[i].kind)));
            btn.set("rect", rectToJs(geom.buttons[i].rect));
            ev::Persistent btnVal(btn.build());
            ev::setElement(buttonsArr.get(), static_cast<uint32_t>(i), btnVal.get());
        }
        res.set("buttons", buttonsArr.get());

        Margins shadowM = geom.shadow_margins;
        ObjectBuilder sm;
        sm.set("left", static_cast<double>(shadowM.left));
        sm.set("right", static_cast<double>(shadowM.right));
        sm.set("top", static_cast<double>(shadowM.top));
        sm.set("bottom", static_cast<double>(shadowM.bottom));
        res.set("shadowMargins", sm.build());

        return res.build();
    });

    // bro.decor.hitTest(geomObj, pointObj)
    decor.def("hitTest", 2, [](Value, std::span<const Value> args) -> Value {
        if (args.size() < 2) return ev::null();
        ev::Persistent geomVal(args[0]);
        ev::Persistent ptVal(args[1]);

        if (!ev::isObject(geomVal.get()) || !ev::isObject(ptVal.get())) return ev::null();

        Vec2 pt = pointFromJs(ptVal.get());

        // Reconstruct FrameGeometry for hit testing
        FrameGeometry geom;
        geom.frame_rect = rectFromJs(ev::getProperty(geomVal.get(), "frameRect"));
        geom.client_rect = rectFromJs(ev::getProperty(geomVal.get(), "clientRect"));
        geom.caption_rect = rectFromJs(ev::getProperty(geomVal.get(), "captionRect"));
        geom.title_rect = rectFromJs(ev::getProperty(geomVal.get(), "titleRect"));
        geom.icon_rect = rectFromJs(ev::getProperty(geomVal.get(), "iconRect"));
        geom.shadow_rect = rectFromJs(ev::getProperty(geomVal.get(), "shadowRect"));

        Value stVal = ev::getProperty(geomVal.get(), "state");
        if (ev::isString(stVal)) {
            geom.state = frameStateFromString(ev::toUtf8(stVal));
        }

        ev::Persistent gripsVal(ev::getProperty(geomVal.get(), "grips"));
        if (ev::isObject(gripsVal.get())) {
            geom.grip_top = rectFromJs(ev::getProperty(gripsVal.get(), "top"));
            geom.grip_bottom = rectFromJs(ev::getProperty(gripsVal.get(), "bottom"));
            geom.grip_left = rectFromJs(ev::getProperty(gripsVal.get(), "left"));
            geom.grip_right = rectFromJs(ev::getProperty(gripsVal.get(), "right"));
            geom.grip_top_left = rectFromJs(ev::getProperty(gripsVal.get(), "topLeft"));
            geom.grip_top_right = rectFromJs(ev::getProperty(gripsVal.get(), "topRight"));
            geom.grip_bottom_left = rectFromJs(ev::getProperty(gripsVal.get(), "bottomLeft"));
            geom.grip_bottom_right = rectFromJs(ev::getProperty(gripsVal.get(), "bottomRight"));
        }

        ev::Persistent btnArr(ev::getProperty(geomVal.get(), "buttons"));
        if (ev::isObject(btnArr.get())) {
            uint32_t len = static_cast<uint32_t>(ev::toDouble(ev::getProperty(btnArr.get(), "length")));
            for (uint32_t i = 0; i < len; ++i) {
                ev::Persistent bItem(ev::getElement(btnArr.get(), i));
                if (ev::isUndefined(bItem.get())) {
                    bItem.set(ev::getProperty(btnArr.get(), std::to_string(i)));
                }
                if (ev::isObject(bItem.get())) {
                    FrameGeometry::ButtonItem bg{};
                    std::string kindStr = ev::toUtf8(ev::getProperty(bItem.get(), "kind"));
                    if (kindStr == "Close") bg.kind = ButtonKind::Close;
                    else if (kindStr == "Maximize") bg.kind = ButtonKind::Maximize;
                    else if (kindStr == "Minimize") bg.kind = ButtonKind::Minimize;
                    else if (kindStr == "Restore") bg.kind = ButtonKind::Restore;
                    else if (kindStr == "Menu") bg.kind = ButtonKind::Menu;
                    else if (kindStr == "Custom") bg.kind = ButtonKind::Custom;
                    bg.rect = rectFromJs(ev::getProperty(bItem.get(), "rect"));
                    geom.buttons.push_back(bg);
                }
            }
        }

        HitTestResult hit = geom.hit_test(pt);

        ObjectBuilder res;
        res.set("area", std::string(to_string(hit.area)));
        res.set("edge", std::string(to_string(hit.edge)));
        res.set("isClient", hit.is_client());
        res.set("isCaption", hit.is_caption());
        res.set("isButton", hit.is_button());
        res.set("isResizable", hit.is_resizable());
        res.set("isDraggable", hit.is_draggable());
        res.set("isShadow", hit.is_shadow());
        res.set("buttonIndex", static_cast<double>(hit.button_index));
        if (hit.button) {
            res.set("buttonKind", std::string(to_string(*hit.button)));
        } else {
            res.set("buttonKind", ev::null());
        }

        return res.build();
    });
}

} // namespace brodecor::api
