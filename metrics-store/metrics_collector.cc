// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class MetricsCollectorRootContext : public RootContext {
public:
  explicit MetricsCollectorRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class MetricsCollectorContext : public Context {
public:
  explicit MetricsCollectorContext(uint32_t id, RootContext *root)
      : Context(id, root), root_(static_cast<MetricsCollectorRootContext *>(
                               static_cast<void *>(root))) {}

private:
  MetricsCollectorRootContext *root_;
};

static RegisterContextFactory
    register_MetricsCollectorContext(CONTEXT_FACTORY(MetricsCollectorContext),
                                     ROOT_FACTORY(MetricsCollectorRootContext),
                                     "metrics_collector");
