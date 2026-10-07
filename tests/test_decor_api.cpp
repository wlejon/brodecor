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
            "  // Hit client area\n"
            "  const hitClient = bro.decor.hitTest(geom, { x: 200, y: 200 });\n"
            "  if (!hitClient || !hitClient.isClient) return false;\n"
            "  // Hit caption\n"
            "  const capX = geom.captionRect.x + 10;\n"
            "  const capY = geom.captionRect.y + 10;\n"
            "  const hitCap = bro.decor.hitTest(geom, { x: capX, y: capY });\n"
            "  if (!hitCap || !hitCap.isCaption) return false;\n"
            "  return true;\n"
            "})()\n");
        CHECK(!r.thrown);
        CHECK(ev::toBool(r.value));
        std::cout << "  bro.decor.hitTest() successfully classified client and caption." << std::endl;
    }

    return ::brotest::finish("test_decor_api");
}
