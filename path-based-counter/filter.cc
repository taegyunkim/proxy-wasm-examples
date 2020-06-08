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

  unsigned long getNextSpanId() { return next_span_id_++; }

  std::string name_;

private:
  unsigned long next_span_id_;
};

class AddHeaderContext : public Context {
public:
  explicit AddHeaderContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<AddHeaderRootContext *>(static_cast<void *>(root))) {}

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

  name_ = config.name();
  return true;
}

FilterHeadersStatus AddHeaderContext::onRequestHeaders(uint32_t) {
  if (getRequestHeader("x-wasm-trace-id")->data() == nullptr) {
    addRequestHeader("x-wasm-trace-id", std::to_string(id()));
  }
  LOG_WARN("x-wasm-trace-id: " +
           getRequestHeader("x-wasm-trace-id")->toString());

  std::string parent_span_id = getRequestHeader("x-wasm-span-id")->toString();
  std::string current_span_id =
      root_->name_ + std::to_string(root_->getNextSpanId());

  if (parent_span_id.empty()) {
    LOG_WARN("root: " + current_span_id);
    addRequestHeader("x-wasm-span-id", current_span_id);
  } else {
    LOG_WARN(parent_span_id + " -> " + current_span_id);
    replaceRequestHeader("x-wasm-span-id", current_span_id);
  }

  auto result = getRequestHeaderPairs();
  auto pairs = result->pairs();
  for (const auto &p : pairs) {
    LOG_WARN(std::string(p.first) + std::string(" -> ") +
             std::string(p.second));
  }
  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus AddHeaderContext::onResponseHeaders(uint32_t) {
  auto result = getResponseHeaderPairs();
  auto pairs = result->pairs();
  for (const auto &p : pairs) {
    LOG_WARN(std::string(p.first) + std::string(" -> ") +
             std::string(p.second));
  }

  // Sanity check this filter is installed and running.
  addResponseHeader("hello", "world");
  return FilterHeadersStatus::Continue;
}
