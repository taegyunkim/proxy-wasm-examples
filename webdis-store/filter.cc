// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class WebdisRootContext : public RootContext {
public:
  explicit WebdisRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class WebdisContext : public Context {
public:
  explicit WebdisContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<WebdisRootContext *>(static_cast<void *>(root))) {}

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;

private:
  WebdisRootContext *root_;
};

FilterHeadersStatus WebdisContext::onRequestHeaders(uint32_t) {
  auto context_id = id();
  auto callback = [context_id](uint32_t, size_t body_size, uint32_t) {
    logWarn("in callback");
    getContext(context_id)->setEffectiveContext();
    auto body = getBufferBytes(BufferType::HttpCallResponseBody, 0, body_size);
    logWarn(std::string(body->view()));
  };
  root()->httpCall("webdis_cluster",
                   {{":method", "GET"}, {":path", "/INCR/counter"}}, "a",
                   {{"trailer", "whatever"}}, 1000, callback);
  return FilterHeadersStatus::Continue;
}

static RegisterContextFactory
    register_WebdisContext(CONTEXT_FACTORY(WebdisContext),
                           ROOT_FACTORY(WebdisRootContext), "webdis_http_call");
