// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

class HttpAuthRootContext : public RootContext {
public:
  explicit HttpAuthRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override { return true; }
};

class HttpAuthContext : public Context {
public:
  explicit HttpAuthContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<HttpAuthRootContext *>(static_cast<void *>(root))) {}

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;

private:
  HttpAuthRootContext *root_;
};
static RegisterContextFactory
    register_HttpAuthContext(CONTEXT_FACTORY(HttpAuthContext),
                             ROOT_FACTORY(HttpAuthRootContext),
                             "http_auth");

FilterHeadersStatus HttpAuthContext::onRequestHeaders(uint32_t) {
  auto token = getRequestHeader("token");
  if (token != nullptr && token->data() != nullptr) {
    logInfo("Auth header : " + token->toString());
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

    if (body->toString() == "Authorized") {
      continueRequest();
    } else {
      sendLocalResponse(403, "", "Acceess forbidden",
                        {{"Powered-by", "proxy-wasm"}});
    }
  };

  root()->httpCall("wasm_upstream",
                   {{":method", "GET"},
                    {":path", "/auth"},
                    {":authority", "wasm_upstream"},
                    {"token", token->toString()}},
                   "", {}, 5000, callback);

  return FilterHeadersStatus::StopIteration;
}
