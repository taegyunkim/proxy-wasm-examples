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
    counter_ = Counter<>::New("istio_path_based_requests");
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
  std::string workload_name;
  if (!getValue({"node", "metadata", "WORKLOAD_NAME"}, &workload_name)) {
    LOG_WARN("Workload name not found");
    return FilterHeadersStatus::Continue;
  }

  // NOTE: Custom headers are not auto-coalesced, so manually accumulate as
  // following. For more details refer:
  // https://github.com/envoyproxy/envoy/issues/9221
  auto cumulative_path = getRequestHeader("x-wasm-path");
  if (cumulative_path->data() == nullptr) {
    addRequestHeader("x-wasm-path", workload_name);
    LOG_WARN("x-wasm-path:" + workload_name);
  } else {
    std::string path = cumulative_path->toString() + "," + workload_name;

    replaceRequestHeader("x-wasm-path", path);
    LOG_WARN("x-wasm-path:" + path);

    if (path == "frontend,recommendationservice,productcatalogservice") {
      LOG_WARN("Incremented counter.");
      root_->counter_->increment(1);
    }
  }

  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus PathBasedCounterContext::onResponseHeaders(uint32_t) {
  // auto result = getResponseHeaderPairs();
  // auto pairs = result->pairs();
  // for (const auto &p : pairs) {
  //   LOG_WARN(std::string(p.first) + std::string(" -> ") +
  //            std::string(p.second));
  // }

  // Sanity check this filter is installed and running.
  addResponseHeader("hello", "world");
  return FilterHeadersStatus::Continue;
}
