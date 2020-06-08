// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class RpcCountRootContext : public RootContext {
public:
  explicit RpcCountRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {
    counter_ = Counter<std::string>::New("simple_counter", "events");
  }

  Counter<std::string> *counter_;
};

class RpcCountContext : public Context {
public:
  explicit RpcCountContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<RpcCountRootContext *>(static_cast<void *>(root))) {}

  FilterStatus onNewConnection() override;
  FilterHeadersStatus onRequestHeaders(uint32_t) override;

private:
  RpcCountRootContext *root_;
};

static RegisterContextFactory
    register_RpcCountContext(CONTEXT_FACTORY(RpcCountContext),
                             ROOT_FACTORY(RpcCountRootContext),
                             "rpc_count_root_id");

FilterStatus RpcCountContext::onNewConnection() {
  root_->counter_->increment(1, "new_connection");
  return FilterStatus::Continue;
}

FilterHeadersStatus RpcCountContext::onRequestHeaders(uint32_t) {
  root_->counter_->increment(1, "request_header");
  return FilterHeadersStatus::Continue;
}
