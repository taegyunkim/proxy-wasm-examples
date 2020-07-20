// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class SingletonRootContext : public RootContext {
public:
  explicit SingletonRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class SingletonContext : public Context {
public:
  explicit SingletonContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<SingletonRootContext *>(static_cast<void *>(root))) {}

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;

private:
  SingletonRootContext *root_;
};

static RegisterContextFactory
    register_SingletonContext(CONTEXT_FACTORY(SingletonContext),
                              ROOT_FACTORY(SingletonRootContext),
                              "singleton-http-call");

FilterHeadersStatus SingletonContext::onRequestHeaders(uint32_t) {
  return FilterHeadersStatus::Continue;
}
