#pragma once

#include "brodecor/decor.h"
#include "brodecor/types.h"

#include <memory>

namespace brodecor::api {

/// Mounts `bro.decor` in the current Bronze realm.
void installDecor();

/// Flushes frame ticks for active decor engines.
void tickDecorAsync();

/// Cleans up any registered decor resources.
void shutdownDecorAsync();

/// Set a custom shared DecoratorEngine used by default helpers.
void setDecoratorEngine(std::shared_ptr<brodecor::DecoratorEngine> engine);

/// Get the shared DecoratorEngine.
std::shared_ptr<brodecor::DecoratorEngine> getDecoratorEngine();

} // namespace brodecor::api

using brodecor::api::installDecor;
using brodecor::api::tickDecorAsync;
using brodecor::api::shutdownDecorAsync;
using brodecor::api::setDecoratorEngine;
using brodecor::api::getDecoratorEngine;
