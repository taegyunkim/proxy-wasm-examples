// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "proxy_wasm_intrinsics.h"

class NullResponseRootContext : public RootContext {
public:
  explicit NullResponseRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override;

  bool onStart(size_t) override;

  std::string header_name_;
  std::string header_value_;
};

class NullResponseContext : public Context {
public:
  explicit NullResponseContext(uint32_t id, RootContext *root)
      : Context(id, root), root_(static_cast<NullResponseRootContext *>(
                               static_cast<void *>(root))) {}

  void onCreate() override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;
  void onDone() override;
  void onLog() override;
  void onDelete() override;

private:
  NullResponseRootContext *root_;
};
static RegisterContextFactory
    register_NullResponseContext(CONTEXT_FACTORY(NullResponseContext),
                                 ROOT_FACTORY(NullResponseRootContext),
                                 "null_response_root_id");

bool NullResponseRootContext::onConfigure(size_t) {
  auto conf = getConfiguration();
  Config config;

  google::protobuf::util::JsonParseOptions options;
  options.case_insensitive_enum_parsing = true;
  options.ignore_unknown_fields = false;

  google::protobuf::util::JsonStringToMessage(conf->toString(), &config,
                                              options);
  LOG_DEBUG("onConfigure name " + config.name());
  LOG_DEBUG("onConfigure " + config.value());
  header_name_ = config.name();
  header_value_ = config.value();
  return true;
}

bool NullResponseRootContext::onStart(size_t) {
  LOG_DEBUG("onStart");
  return true;
}

void NullResponseContext::onCreate() {
  LOG_DEBUG(std::string("onCreate " + std::to_string(id())));
}

FilterHeadersStatus NullResponseContext::onResponseHeaders(uint32_t) {
  replaceResponseHeader("Transfer-Encoding", "gzip");
  return FilterHeadersStatus::Continue;
}

void NullResponseContext::onDone() {
  LOG_DEBUG(std::string("onDone " + std::to_string(id())));
}

void NullResponseContext::onLog() {
  LOG_DEBUG(std::string("onLog " + std::to_string(id())));
}

void NullResponseContext::onDelete() {
  LOG_DEBUG(std::string("onDelete " + std::to_string(id())));
}
