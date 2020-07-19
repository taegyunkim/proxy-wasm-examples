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

  FilterStatus onDownstreamData(size_t, bool) override;
  FilterStatus onUpstreamData(size_t, bool) override;
  void onDownstreamConnectionClose(PeerType) override;
  void onUpstreamConnectionClose(PeerType) override;

private:
  size_t data_downstream_ = 0;
  size_t data_upstream_ = 0;
  uint64_t upstream_close_time_ = 0;
  uint64_t latency_ = 0;

  TcpMetricsRootContext *root_;
};

static RegisterContextFactory
    register_TcpMetricsContext(CONTEXT_FACTORY(TcpMetricsContext),
                               ROOT_FACTORY(TcpMetricsRootContext),
                               "tcp_metrics");

FilterStatus TcpMetricsContext::onDownstreamData(size_t data_size, bool) {
  data_downstream_ += data_size;
  return FilterStatus::Continue;
}

FilterStatus TcpMetricsContext::onUpstreamData(size_t data_size, bool) {
  data_upstream_ += data_size;
  return FilterStatus::Continue;
}

void TcpMetricsContext::onDownstreamConnectionClose(PeerType) {
  uint64_t curr_time = getCurrentTimeNanoseconds();

  latency_ = curr_time - upstream_close_time_;

  logInfo("data_downstream_: " + std::to_string(data_downstream_) +
          " data_upstream_: " + std::to_string(data_upstream_) +
          " upstream_close_time_: " + std::to_string(upstream_close_time_) +
          " latency_: " + std::to_string(latency_));
}

void TcpMetricsContext::onUpstreamConnectionClose(PeerType) {
  uint64_t curr_time = getCurrentTimeNanoseconds();

  upstream_close_time_ = curr_time;
}