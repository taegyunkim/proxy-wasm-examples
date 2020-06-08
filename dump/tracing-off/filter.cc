// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "proxy_wasm_intrinsics.h"

class AddHeaderRootContext : public RootContext {
public:
  explicit AddHeaderRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override;
};

class AddHeaderContext : public Context {
public:
  explicit AddHeaderContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<AddHeaderRootContext *>(static_cast<void *>(root))) {}

  void onCreate() override;
  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;

private:
  AddHeaderRootContext *root_;
};
static RegisterContextFactory
    register_AddHeaderContext(CONTEXT_FACTORY(AddHeaderContext),
                              ROOT_FACTORY(AddHeaderRootContext),
                              "add_header_root_id");

bool AddHeaderRootContext::onConfigure(size_t) {
  auto conf = getConfiguration();
  Config config;

  google::protobuf::util::JsonParseOptions options;
  options.case_insensitive_enum_parsing = true;
  options.ignore_unknown_fields = false;

  google::protobuf::util::JsonStringToMessage(conf->toString(), &config,
                                              options);
  LOG_DEBUG("onConfigure name " + config.name());
  LOG_DEBUG("onConfigure " + config.value());
  return true;
}

void AddHeaderContext::onCreate() {
  LOG_DEBUG(std::string("onCreate " + std::to_string(id())));
}

FilterHeadersStatus AddHeaderContext::onRequestHeaders(uint32_t) {
  removeRequestHeader("x-request-id");
  removeRequestHeader("x-b3-traceid");
  removeRequestHeader("x-b3-spanid");
  removeRequestHeader("x-b3-parentspanid");
  removeRequestHeader("x-b3-sampled");
  removeRequestHeader("x-b3-flags");
  removeRequestHeader("b3");

  auto result = getRequestHeaderPairs();
  auto pairs = result->pairs();
  for (const auto &p : pairs) {
    LOG_WARN(std::string(p.first) + std::string(" -> ") +
             std::string(p.second));
  }

  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus AddHeaderContext::onResponseHeaders(uint32_t) {
  addResponseHeader("hello", "world");
  addResponseHeader("tracing", "off");
  return FilterHeadersStatus::Continue;
}
