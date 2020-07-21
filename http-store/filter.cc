// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class HttpStoreRootContext : public RootContext {
public:
  explicit HttpStoreRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
};

class HttpStoreContext : public Context {
public:
  explicit HttpStoreContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<HttpStoreRootContext *>(static_cast<void *>(root))) {}

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;

private:
  HttpStoreRootContext *root_;
};

static RegisterContextFactory
    register_HttpStoreContext(CONTEXT_FACTORY(HttpStoreContext),
                              ROOT_FACTORY(HttpStoreRootContext), "http_store");

FilterHeadersStatus HttpStoreContext::onRequestHeaders(uint32_t) {
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
  logWarn("onRequestHeaders");

  auto trace_id = getRequestHeader("x-b3-traceid");
  if (trace_id != nullptr && trace_id->data() != nullptr) {
    uint64_t curr_time = getCurrentTimeNanoseconds();
    root()->httpCall("storage-upstream",
                     {{":method", "GET"},
                      {":path", "/store"},
                      {":authority", "storage-upstream"},
                      {"key", trace_id->toString()},
                      {"value", std::to_string(curr_time)}},
                     "", {}, 1000, callback);
  }
  return FilterHeadersStatus::Continue;
}
