// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class SingletonQueueRootContext : public RootContext {
public:
  explicit SingletonQueueRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class SingletonQueueContext : public Context {
public:
  explicit SingletonQueueContext(uint32_t id, RootContext *root)
      : Context(id, root), root_(static_cast<SingletonQueueRootContext *>(
                               static_cast<void *>(root))) {}

private:
  SingletonQueueRootContext *root_;
};

static RegisterContextFactory
    register_SingletonQueueContext(CONTEXT_FACTORY(SingletonQueueContext),
                                   ROOT_FACTORY(SingletonQueueRootContext),
                                   "singleton_queue");
