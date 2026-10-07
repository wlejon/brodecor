#include "api.h"
#include "object_builder.h"

#include <mutex>

namespace brodecor::api {

void installDecorOnto(Value decorVal);

namespace {

std::mutex g_decor_mu;
std::shared_ptr<brodecor::DecoratorEngine> g_custom_engine;
std::shared_ptr<brodecor::DecoratorEngine> g_default_engine;

} // namespace

std::shared_ptr<brodecor::DecoratorEngine> activeDecoratorEngine() {
    std::lock_guard lock(g_decor_mu);
    if (g_custom_engine) return g_custom_engine;
    if (!g_default_engine) {
        g_default_engine = std::make_shared<brodecor::DecoratorEngine>();
    }
    return g_default_engine;
}

void setDecoratorEngine(std::shared_ptr<brodecor::DecoratorEngine> engine) {
    std::lock_guard lock(g_decor_mu);
    g_custom_engine = std::move(engine);
}

std::shared_ptr<brodecor::DecoratorEngine> getDecoratorEngine() {
    return activeDecoratorEngine();
}

Value ensureBroDecor() {
    ev::Persistent globalThisVal;
    auto gt = ev::globalValue("globalThis");
    if (gt.found && ev::isObject(gt.value)) {
        globalThisVal.set(gt.value);
    }

    ev::Persistent broP;
    auto bro = ev::globalValue("bro");
    if (bro.found && ev::isObject(bro.value)) broP.set(bro.value);
    if (!ev::isObject(broP.get()) && ev::isObject(globalThisVal.get())) {
        Value candidate = ev::getProperty(globalThisVal.get(), "bro");
        if (ev::isObject(candidate)) broP.set(candidate);
    }
    if (!ev::isObject(broP.get())) {
        broP.set(ev::createObject());
        ev::registerGlobal("bro", broP.get());
        if (ev::isObject(globalThisVal.get())) {
            globalThisVal.set(ev::setProperty(globalThisVal.get(), "bro", broP.get()));
        }
    }

    ev::Persistent decorP(ev::getProperty(broP.get(), "decor"));
    if (!ev::isObject(decorP.get())) {
        decorP.set(ev::createObject());
        broP.set(ev::setProperty(broP.get(), "decor", decorP.get()));
    }
    return decorP.get();
}

void installDecor() {
    ev::Persistent decorObj(ensureBroDecor());
    installDecorOnto(decorObj.get());
}

void tickDecorAsync() {
    // No background asynchronous event queue needed for synchronous geometry math
}

void shutdownDecorAsync() {
    std::lock_guard lock(g_decor_mu);
    g_custom_engine.reset();
    g_default_engine.reset();
}

} // namespace brodecor::api
