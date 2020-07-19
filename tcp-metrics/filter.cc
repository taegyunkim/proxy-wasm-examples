// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class TcpMetricsRootContext : public RootContext {
public:
  explicit TcpMetricsRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class TcpMetricsContext : public Context {
public:
  explicit TcpMetricsContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<TcpMetricsRootContext *>(static_cast<void *>(root))) {
  }

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;

private:
  TcpMetricsRootContext *root_;
};
static RegisterContextFactory
    register_TcpMetricsContext(CONTEXT_FACTORY(TcpMetricsContext),
                               ROOT_FACTORY(TcpMetricsRootContext),
                               "tcp_metrics");

FilterHeadersStatus TcpMetricsContext::onRequestHeaders(uint32_t) {
  return FilterHeadersStatus::Continue;
}
