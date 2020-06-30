// NOLINT(namespace-envoy)
#include <map>
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

// TrafficDirection is a mirror of envoy xDS traffic direction.
// As defined in istio/proxy/extensions/common/context.h
enum class TrafficDirection : int64_t {
  Unspecified = 0,
  Inbound = 1,
  Outbound = 2,
};

// Retrieves the traffic direction from the configuration context.
TrafficDirection getTrafficDirection() {
  int64_t direction;
  if (getValue({"listener_direction"}, &direction)) {
    return static_cast<TrafficDirection>(direction);
  }
  return TrafficDirection::Unspecified;
}

std::string trafficDirectionToString(TrafficDirection dir) {
  if (dir == TrafficDirection::Unspecified) {
    return "unspecified";
  } else if (dir == TrafficDirection::Inbound) {
    return "inbound";
  } else {
    return "outbound";
  }
}

class BidiRootContext : public RootContext {
public:
  explicit BidiRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override;
};

class BidiContext : public Context {
public:
  explicit BidiContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<BidiRootContext *>(static_cast<void *>(root))),
        b3_trace_id_(""), b3_span_id_(""), b3_parent_span_id_(""),
        grpc_trace_bin_("") {
    direction_ = getTrafficDirection();
  }

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;

private:
  BidiRootContext *root_;
  std::string b3_trace_id_;
  std::string b3_span_id_;
  std::string b3_parent_span_id_;
  std::string grpc_trace_bin_;
  TrafficDirection direction_;
};

static RegisterContextFactory
    register_BidiContext(CONTEXT_FACTORY(BidiContext),
                         ROOT_FACTORY(BidiRootContext), "bidi_root_id");

bool BidiRootContext::onConfigure(size_t) { return true; }

FilterHeadersStatus BidiContext::onRequestHeaders(uint32_t) {
  // auto header_pairs = getRequestHeaderPairs()->pairs();
  // for (const auto &pair : header_pairs) {
  //   LOG_WARN(trafficDirectionToString(direction_) + ": " +
  //            std::string(pair.first) + " --> " + std::string(pair.second));
  // }

  // auto grpc_trace_bin = getRequestHeader("grpc-trace-bin");
  // if (grpc_trace_bin->data() != nullptr) {
  //   grpc_trace_bin_ = grpc_trace_bin->toString();
  //   LOG_WARN(trafficDirectionToString(direction_) + ": " + grpc_trace_bin_);
  // } else {
  //   LOG_WARN(trafficDirectionToString(direction_) +
  //            " grpc-trace-bin not found");
  // }

  auto trace_id = getRequestHeader("x-b3-traceid");
  if (trace_id->data() == nullptr) {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-traceid not found!");
  } else {
    b3_trace_id_ = trace_id->toString();
    LOG_WARN(trafficDirectionToString(direction_) + " trace_id_ " +
             b3_trace_id_);
  }

  auto span_id = getRequestHeader("x-b3-spanid");
  if (span_id->data() == nullptr) {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-spanid not found!");

  } else {
    b3_span_id_ = span_id->toString();
    LOG_WARN(trafficDirectionToString(direction_) + " span_id " + b3_span_id_);
  }

  auto parent_span_id = getRequestHeader("x-b3-parentspanid");
  if (parent_span_id->data() == nullptr) {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-parentspanid not found!");
  } else {
    b3_parent_span_id_ = parent_span_id->toString();
    LOG_WARN(trafficDirectionToString(direction_) + " parent_span_id " +
             b3_parent_span_id_);
  }

  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus BidiContext::onResponseHeaders(uint32_t) {
  // if (grpc_trace_bin_ != "") {
  //   LOG_WARN(trafficDirectionToString(direction_) + ": " + grpc_trace_bin_);
  // } else {
  //   LOG_WARN(trafficDirectionToString(direction_) +
  //            " grpc-trace-bin not found");
  // }

  // auto header_pairs = getResponseHeaderPairs()->pairs();
  // for (const auto &pair : header_pairs) {
  //   LOG_WARN(trafficDirectionToString(direction_) + ": " +
  //            std::string(pair.first) + " --> " + std::string(pair.second));
  // }

  if (b3_trace_id_ == "") {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-traceid not set");
  } else {
    LOG_WARN(trafficDirectionToString(direction_) + " trace_id " +
             b3_trace_id_);
  }

  if (b3_span_id_ == "") {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-spanid not set");
  } else {
    LOG_WARN(trafficDirectionToString(direction_) + " span_id " + b3_span_id_);
  }

  if (b3_parent_span_id_ == "") {
    LOG_WARN(trafficDirectionToString(direction_) + " " +
             "x-b3-parentspanid not set");
  } else {
    LOG_WARN(trafficDirectionToString(direction_) + " parent_span_id " +
             b3_parent_span_id_);
  }

  if (direction_ == TrafficDirection::Inbound) {
    std::string workload_name;
    if (!getValue({"node", "metadata", "WORKLOAD_NAME"}, &workload_name)) {
      LOG_WARN("inbound: Workload name not found");
      return FilterHeadersStatus::Continue;
    }

    // inbound response processing
    WasmDataPtr shared_data;
    WasmResult result = getSharedData(b3_span_id_, &shared_data);
    if (result == WasmResult::Ok && shared_data->data() != nullptr) {
      auto header_value = shared_data->toString();
      addResponseHeader("x-wasm", workload_name + "-" + header_value);
      LOG_WARN("inbound: x-wasm -> " + workload_name + "-" + header_value);
    } else {
      addResponseHeader("x-wasm", workload_name);
      LOG_WARN("inbound: x-wasm -> " + workload_name);
    }
  } else if (direction_ == TrafficDirection::Outbound) {
    // Received response from another service we called.
    // Collect x-wasm header value.
    auto header = getResponseHeader("x-wasm");
    if (header->data() != nullptr) {
      WasmDataPtr shared_data;
      WasmResult result = getSharedData(b3_parent_span_id_, &shared_data);
      if (result == WasmResult::Ok && shared_data->data() != nullptr) {
        LOG_WARN("outbound: x-wasm -> " + shared_data->toString() + "," +
                 header->toString());
        setSharedData(b3_parent_span_id_,
                      shared_data->toString() + "," + header->toString());
      } else {
        LOG_WARN("outbound: x-wasm -> " + header->toString());
        setSharedData(b3_parent_span_id_, header->toString());
      }
    } else {
      LOG_WARN("outbound: x-wasm not found");
    }
  }

  return FilterHeadersStatus::Continue;
}
