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

  FilterStatus onDownstreamData(size_t data_size,
                                bool /*end_of_stream*/) override;

private:
  SingletonRootContext *root_;
};

FilterStatus SingletonContext::onDownstreamData(size_t data_size,
                                                bool /*end_of_stream*/) {
  auto data = getBufferBytes(BufferType::NetworkDownstreamData, 0, data_size);
  logDebug(data->toString());
  return FilterStatus::Continue;
}

static RegisterContextFactory
    register_SingletonContext(CONTEXT_FACTORY(SingletonContext),
                              ROOT_FACTORY(SingletonRootContext),
                              "tcp_packet_parse");
