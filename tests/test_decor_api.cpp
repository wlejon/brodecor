#include "../src/api/api.h"
#include "embed/embed.h"
#include "eval/eval.h"
#include "check.h"

#include <iostream>

int main() {
    namespace ev = bronze::embed;
    using namespace bronze::eval;

    std::cout << "Starting brodecor JavaScript API test..." << std::endl;

    // 1. Install bro.decor into Bronze realm
    brodecor::api::installDecor();

    auto g = ev::globalValue("bro");
    CHECK(g.found);
    CHECK(ev::isObject(g.value));

    ev::Persistent decor(ev::getProperty(g.value, "decor"));
    CHECK(ev::isObject(decor.get()));
    std::cout << "  Mounted bro.decor successfully." << std::endl;

    // 2. Test getDefaultMetrics()
    {
        auto r = evalScript(
            "(function() {\n"
            "  const m = bro.decor.getDefaultMetrics();\n"
            "  return m.captionHeight > 0 && m.borderThickness > 0 && m.buttonWidth > 0;\n"
            "})()\n");
        CHECK(!r.thrown);
        CHECK(ev::toBool(r.value));
        std::cout << "  bro.decor.getDefaultMetrics() returned valid metrics." << std::endl;
    }

    // 3. Test computeGeometry()
    {
        auto r = evalScript(
            "(function() {\n"
            "  const geom = bro.decor.computeGeometry({\n"
            "    clientRect: { x: 100, y: 100, width: 800, height: 600 },\n"
            "    title: 'Test Window',\n"
            "    state: 'active'\n"
            "  });\n"
            "  if (!geom) return false;\n"
            "  if (geom.frameRect.width <= geom.clientRect.width) return false;\n"
            "  if (geom.captionRect.height <= 0) return false;\n"
            "  if (!Array.isArray(geom.buttons) || geom.buttons.length === 0) return false;\n"
            "  return true;\n"
            "})()\n");
        CHECK(!r.thrown);
        CHECK(ev::toBool(r.value));
        std::cout << "  bro.decor.computeGeometry() calculated frame geometry properly." << std::endl;
    }

    // 4. Test hitTest()
    {
        auto r = evalScript(
            "(function() {\n"
            "  const geom = bro.decor.computeGeometry({\n"
            "    clientRect: { x: 100, y: 100, width: 800, height: 600 },\n"
            "    title: 'Test Window'\n"
            "  });\n"
            "  if (geom.state !== 'active') return false;\n"
            "  if (!geom.shadowRect || geom.shadowRect.width <= geom.frameRect.width) return false;\n"
            "  if (!geom.grips || !geom.grips.topLeft || !geom.grips.top) return false;\n"
            "  // Hit client area\n"
            "  const hitClient = bro.decor.hitTest(geom, { x: 200, y: 200 });\n"
            "  if (!hitClient || !hitClient.isClient) return false;\n"
            "  // Hit caption (below top grip)\n"
            "  const capX = geom.captionRect.x + 40;\n"
            "  const capY = geom.captionRect.y + 15;\n"
            "  const hitCap = bro.decor.hitTest(geom, { x: capX, y: capY });\n"
            "  if (!hitCap || !hitCap.isCaption) return false;\n"
            "  // Hit topLeft corner grip\n"
            "  const hitTL = bro.decor.hitTest(geom, { x: geom.frameRect.x, y: geom.frameRect.y });\n"
            "  if (!hitTL || !hitTL.isResizable || hitTL.area !== 'ResizeBorder' || hitTL.edge !== 'TopLeft') return false;\n"
            "  // Hit bottomRight corner grip\n"
            "  const hitBR = bro.decor.hitTest(geom, { x: geom.frameRect.x + geom.frameRect.width - 1, y: geom.frameRect.y + geom.frameRect.height - 1 });\n"
            "  if (!hitBR || !hitBR.isResizable || hitBR.edge !== 'BottomRight') return false;\n"
            "  // Hit top resize border overlapping top of frame/caption\n"
            "  const hitTop = bro.decor.hitTest(geom, { x: geom.frameRect.x + 100, y: geom.frameRect.y + 1 });\n"
            "  if (!hitTop || !hitTop.isResizable || hitTop.area !== 'ResizeBorder' || hitTop.edge !== 'Top') return false;\n"
            "  // Hit shadow area outside frameRect and grips\n"
            "  const hitShadow = bro.decor.hitTest(geom, { x: geom.shadowRect.x + 2, y: geom.shadowRect.y + 2 });\n"
            "  if (!hitShadow || hitShadow.area !== 'Shadow' || !hitShadow.isShadow) return false;\n"
            "  return true;\n"
            "})()\n");
        CHECK(!r.thrown);
        CHECK(ev::toBool(r.value));
        std::cout << "  bro.decor.hitTest() successfully classified client, caption, corner/edge grips, and shadow." << std::endl;
    }

    // 5. Test hitTest() for Maximized Restore button and Custom button kinds
    {
        auto r = evalScript(
            "(function() {\n"
            "  const geom = bro.decor.computeGeometry({\n"
            "    clientRect: { x: 0, y: 32, width: 1920, height: 1048 },\n"
            "    title: 'Maximized Window',\n"
            "    state: 'maximized'\n"
            "  });\n"
            "  if (geom.state !== 'maximized') return false;\n"
            "  let restoreBtn = null;\n"
            "  for (let i = 0; i < geom.buttons.length; ++i) {\n"
            "    if (geom.buttons[i].kind === 'Restore') restoreBtn = geom.buttons[i];\n"
            "  }\n"
            "  if (!restoreBtn) return false;\n"
            "  const hitRestore = bro.decor.hitTest(geom, {\n"
            "    x: restoreBtn.rect.x + 5,\n"
            "    y: restoreBtn.rect.y + 5\n"
            "  });\n"
            "  if (!hitRestore || !hitRestore.isButton || hitRestore.buttonKind !== 'Restore') return false;\n"
            "  // Maximized window top edge should not be resizable\n"
            "  const hitTop = bro.decor.hitTest(geom, { x: 200, y: geom.frameRect.y + 1 });\n"
            "  if (!hitTop || hitTop.isResizable || hitTop.area !== 'Caption') return false;\n"
            "  // Custom button kind round-trip\n"
            "  geom.buttons.push({ kind: 'Custom', rect: { x: 50, y: 4, width: 24, height: 24 } });\n"
            "  const hitCustom = bro.decor.hitTest(geom, { x: 55, y: 10 });\n"
            "  if (!hitCustom || !hitCustom.isButton || hitCustom.buttonKind !== 'Custom') return false;\n"
            "  return true;\n"
            "})()\n");
        if (r.thrown) {
            std::cerr << "Exception: " << ev::toUtf8(r.value) << std::endl;
        }
        CHECK(!r.thrown);
        CHECK(ev::toBool(r.value));
        std::cout << "  bro.decor.hitTest() handled maximized Restore and Custom buttons properly." << std::endl;
    }

    return ::brotest::finish("test_decor_api");
}
