// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "proxy_wasm_intrinsics.h"

class ParallelPathRootContext : public RootContext {
public:
  explicit ParallelPathRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override;

  bool onStart(size_t) override;

  std::string header_name_;
  std::string header_value_;
};

class ParallelPathContext : public Context {
public:
  explicit ParallelPathContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<ParallelPathRootContext *>(static_cast<void *>(root))) {}

  void onCreate() override;
  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;
  FilterDataStatus onRequestBody(size_t body_buffer_length,
                                 bool end_of_stream) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;
  void onDone() override;
  void onLog() override;
  void onDelete() override;

private:
  ParallelPathRootContext *root_;
};
static RegisterContextFactory
    register_ParallelPathContext(CONTEXT_FACTORY(ParallelPathContext),
                              ROOT_FACTORY(ParallelPathRootContext),
                              "add_header_root_id");

bool ParallelPathRootContext::onConfigure(size_t) {
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

bool ParallelPathRootContext::onStart(size_t) {
  LOG_DEBUG("onStart");
  return true;
}

void ParallelPathContext::onCreate() {
  LOG_DEBUG(std::string("onCreate " + std::to_string(id())));
}

FilterHeadersStatus ParallelPathContext::onRequestHeaders(uint32_t) {
  LOG_DEBUG(std::string("onRequestHeaders ") + std::to_string(id()));
  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus ParallelPathContext::onResponseHeaders(uint32_t) {
  LOG_DEBUG(std::string("onResponseHeaders ") + std::to_string(id()));
  addResponseHeader(root_->header_name_, root_->header_value_);
  replaceResponseHeader("location", "envoy-wasm");
  return FilterHeadersStatus::Continue;
}

FilterDataStatus ParallelPathContext::onRequestBody(size_t body_buffer_length,
                                                 bool end_of_stream) {
  return FilterDataStatus::Continue;
}

void ParallelPathContext::onDone() {
  LOG_DEBUG(std::string("onDone " + std::to_string(id())));
}

void ParallelPathContext::onLog() {
  LOG_DEBUG(std::string("onLog " + std::to_string(id())));
}

void ParallelPathContext::onDelete() {
  LOG_DEBUG(std::string("onDelete " + std::to_string(id())));
}
