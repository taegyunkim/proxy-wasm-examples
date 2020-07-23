// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class NatsCRootContext : public RootContext {
public:
  explicit NatsCRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class NatsCContext : public Context {
public:
  explicit NatsCContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<NatsCRootContext *>(static_cast<void *>(root))) {}

private:
  NatsCRootContext *root_;
};

static RegisterContextFactory
    register_NatsCContext(CONTEXT_FACTORY(NatsCContext),
                          ROOT_FACTORY(NatsCRootContext), "nats_c");
