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
                             "http_auth_root_id");

FilterHeadersStatus HttpAuthContext::onRequestHeaders(uint32_t) {
  return FilterHeadersStatus::Continue;
}
