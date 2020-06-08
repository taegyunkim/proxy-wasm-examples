// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "proxy_wasm_intrinsics.h"

class PathBasedCounterRootContext : public RootContext {
public:
  explicit PathBasedCounterRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {
    counter_ = Counter<>::New("num_requests");
  }
  bool onConfigure(size_t /* configuration_size */) override;

  unsigned long getNextSpanId() { return next_span_id_++; }

  std::string name_;
  Counter<> *counter_;

private:
  unsigned long next_span_id_;
};

class PathBasedCounterContext : public Context {
public:
  explicit PathBasedCounterContext(uint32_t id, RootContext *root)
      : Context(id, root), root_(static_cast<PathBasedCounterRootContext *>(
                               static_cast<void *>(root))) {}

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;

private:
  PathBasedCounterRootContext *root_;
};

static RegisterContextFactory
    register_PathBasedCounterContext(CONTEXT_FACTORY(PathBasedCounterContext),
                                     ROOT_FACTORY(PathBasedCounterRootContext),
                                     "path_based_counter_root_id");

bool PathBasedCounterRootContext::onConfigure(size_t) {
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

FilterHeadersStatus PathBasedCounterContext::onRequestHeaders(uint32_t) {
  if (getRequestHeader("x-wasm-trace-id")->data() == nullptr) {
    addRequestHeader("x-wasm-trace-id", std::to_string(id()));
  }
  LOG_WARN("x-wasm-trace-id: " +
           getRequestHeader("x-wasm-trace-id")->toString());

  auto parent_span_id = getRequestHeader("x-wasm-span-id");
  std::string current_span_id =
      root_->name_ + std::to_string(root_->getNextSpanId());

  if (parent_span_id->data() == nullptr) {
    LOG_WARN("root: " + current_span_id);
    addRequestHeader("x-wasm-span-id", current_span_id);
  } else {
    LOG_WARN(parent_span_id->toString() + " -> " + current_span_id);
    replaceRequestHeader("x-wasm-span-id", current_span_id);
  }

//   auto result = getRequestHeaderPairs();
//   auto pairs = result->pairs();
//   for (const auto &p : pairs) {
//     LOG_WARN(std::string(p.first) + std::string(" -> ") +
//              std::string(p.second));
//   }

  root_->counter_->increment(1);
  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus PathBasedCounterContext::onResponseHeaders(uint32_t) {
//   auto result = getResponseHeaderPairs();
//   auto pairs = result->pairs();
//   for (const auto &p : pairs) {
//     LOG_WARN(std::string(p.first) + std::string(" -> ") +
//              std::string(p.second));
//   }

  // Sanity check this filter is installed and running.
  addResponseHeader("hello", "world");
  return FilterHeadersStatus::Continue;
}
