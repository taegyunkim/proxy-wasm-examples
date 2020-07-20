// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class SingletonRootContext : public RootContext {
public:
  explicit SingletonRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}

  bool onStart(size_t) override;

  void onTick() override;
};

bool SingletonRootContext::onStart(size_t /*vm_configuration_size*/) {
  logDebug("VM instantiated");
  proxy_set_tick_period_milliseconds(5000);
  return true;
}

void SingletonRootContext::onTick() {
  auto callback = [](uint32_t headers, size_t body_size, uint32_t trailers) {
    auto body = getBufferBytes(BufferType::HttpCallResponseBody, 0, body_size);
    logInfo("Http Call Response: " + body->toString());
  };

  auto result = httpCall(
      "wasm_upstream",
      {{":method", "GET"}, {":path", "/auth"}, {":authority", "wasm_upstream"}},
      "", {}, 5000, callback);
}

class SingletonContext : public Context {
public:
  explicit SingletonContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<SingletonRootContext *>(static_cast<void *>(root))) {}

private:
  SingletonRootContext *root_;
};

static RegisterContextFactory
    register_SingletonContext(CONTEXT_FACTORY(SingletonContext),
                              ROOT_FACTORY(SingletonRootContext),
                              "singleton-http-call");
