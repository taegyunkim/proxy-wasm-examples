// NOLINT(namespace-envoy)
#include <optional>
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class SingletonQueueRootContext : public RootContext {
public:
  explicit SingletonQueueRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}

  bool onStart(size_t) override;
  void onTick() override;

private:
  std::optional<uint32_t> token_ = std::nullopt;
};

bool SingletonQueueRootContext::onStart(size_t) {
  logDebug("onStart");
  proxy_set_tick_period_milliseconds(2000);

  uint32_t token = 0;
  auto result = registerSharedQueue("q1", &token);
  if (result == WasmResult::Ok) {
    logDebug("Registered shared queue: " + std::to_string(token));
    token_.emplace(token);
  } else {
    logDebug("Failed to register shared queue: " + toString(result));
  }

  return true;
}

void SingletonQueueRootContext::onTick() {
  if (!token_.has_value()) {
    logDebug("Queue not initialized");
    return;
  }

  WasmDataPtr data;
  auto result = dequeueSharedQueue(token_.value(), &data);
  if (result != WasmResult::Ok) {
    logDebug("Failed to dequeue: " + toString(result));
    return;
  }

  if (data == nullptr || data->data() == nullptr) {
    logDebug("Returned data nullptr");
    return;
  }

  auto context_id = id();
  auto callback = [context_id](uint32_t, size_t body_size, uint32_t) {
    auto response_headers =
        getHeaderMapPairs(HeaderMapType::HttpCallResponseHeaders);
    // Switch context after getting headers, but before getting body to
    // exercise both code paths.
    getContext(context_id)->setEffectiveContext();
    auto body = getBufferBytes(BufferType::HttpCallResponseBody, 0, body_size);
    auto response_trailers =
        getHeaderMapPairs(HeaderMapType::HttpCallResponseTrailers);
    for (auto &p : response_headers->pairs()) {
      logInfo(std::string(p.first) + std::string(" -> ") +
              std::string(p.second));
    }
    logDebug(std::string(body->view()));
    for (auto &p : response_trailers->pairs()) {
      logInfo(std::string(p.first) + std::string(" -> ") +
              std::string(p.second));
    }
  };

  httpCall("wasm_upstream",
           {{":method", "GET"},
            {":path", "/store"},
            {":authority", "wasm_upstream"}},
           data->toString(), {}, 5000, callback);
}

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
                                   "singleton");
