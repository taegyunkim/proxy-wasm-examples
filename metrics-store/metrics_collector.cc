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

  FilterStatus onDownstreamData(size_t, bool) override;
  FilterStatus onUpstreamData(size_t, bool) override;
  void onDownstreamConnectionClose(PeerType) override;
  void onUpstreamConnectionClose(PeerType) override;

private:
  size_t data_downstream_ = 0;
  size_t data_upstream_ = 0;
  uint64_t time_;
  uint64_t latency_ = 0;

  MetricsCollectorRootContext *root_;
};

FilterStatus MetricsCollectorContext::onDownstreamData(size_t data_size, bool) {
  data_downstream_ += data_size;
  return FilterStatus::Continue;
}

FilterStatus MetricsCollectorContext::onUpstreamData(size_t data_size, bool) {
  data_upstream_ += data_size;
  return FilterStatus::Continue;
}

void MetricsCollectorContext::onDownstreamConnectionClose(PeerType) {
  latency_ = getCurrentTimeNanoseconds() - time_;

  uint32_t token = 0;
  auto result = resolveSharedQueue("singleton", "q1", &token);
  if (result != WasmResult::Ok) {
    logDebug("Failed to resolve shared queue token: " + toString(result));
    return;
  }

  std::string data = std::to_string(data_downstream_) + " | " +
                     std::to_string(data_upstream_) + " | " +
                     std::to_string(latency_);
  result = enqueueSharedQueue(token, data);
  if (result != WasmResult::Ok) {
    logDebug("Failed to enqueue data: " + toString(result));
  }
}

void MetricsCollectorContext::onUpstreamConnectionClose(PeerType) {
  time_ = getCurrentTimeNanoseconds();
}

static RegisterContextFactory
    register_MetricsCollectorContext(CONTEXT_FACTORY(MetricsCollectorContext),
                                     ROOT_FACTORY(MetricsCollectorRootContext),
                                     "metrics_collector");
