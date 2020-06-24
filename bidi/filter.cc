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

class BidiRootContext : public RootContext {
public:
  explicit BidiRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}
  bool onConfigure(size_t /* configuration_size */) override;

  std::map<std::string, std::string> forward_trace_ids_to_headers_;
  std::map<std::string, std::string> backward_trace_ids_to_headers_;
};

class BidiContext : public Context {
public:
  explicit BidiContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<BidiRootContext *>(static_cast<void *>(root))),
        b3_span_id_("") {
    direction_ = getTrafficDirection();
  }

  FilterHeadersStatus onRequestHeaders(uint32_t headers) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers) override;

private:
  BidiRootContext *root_;
  std::string b3_span_id_;
  TrafficDirection direction_;
};

static RegisterContextFactory
    register_BidiContext(CONTEXT_FACTORY(BidiContext),
                         ROOT_FACTORY(BidiRootContext), "bidi_root_id");

bool BidiRootContext::onConfigure(size_t) { return true; }

FilterHeadersStatus BidiContext::onRequestHeaders(uint32_t) {
  auto span_id = getRequestHeader("x-b3-spanid");
  if (span_id == nullptr) {
    LOG_WARN("x-b3-spanid not found!");
    return FilterHeadersStatus::Continue;
  }
  b3_span_id_ = span_id->toString();

  if (direction_ == TrafficDirection::Inbound) {
    // Get workload name
    std::string workload_name;
    if (!getValue({"node", "metadata", "WORKLOAD_NAME"}, &workload_name)) {
      LOG_WARN("inbound RequestHeaders: Workload name not found");
      return FilterHeadersStatus::Continue;
    }

    // Check incoming request has x-wasm header.
    auto header = getRequestHeader("x-wasm");
    std::string header_value = "";
    if (header->data() != nullptr) {
      header_value = header->toString() + "-";
    }
    // Append workload name to x-wasm header
    header_value += workload_name;
    LOG_WARN("inbound RequestHeaders: " + b3_span_id_ + " -> " + header_value);
    // Store it for later.
    root_->forward_trace_ids_to_headers_[b3_span_id_] = header_value;
  } else if (direction_ == TrafficDirection::Outbound) {
    // Retrieve saved x-wasm header using b3_span_id.
    auto entry = root_->forward_trace_ids_to_headers_.find(b3_span_id_);
    if (entry == root_->forward_trace_ids_to_headers_.end()) {
      LOG_WARN("Not found " + b3_span_id_);
      return FilterHeadersStatus::Continue;
    }
    // Append it to request header.
    addRequestHeader("x-wasm", entry->second);
    LOG_WARN("outbound RequestHeaders: x-wasm -> " + entry->second);
  }

  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus BidiContext::onResponseHeaders(uint32_t) {
  if (b3_span_id_ == "") {
    LOG_WARN("x-b3-spanid not set");
    return FilterHeadersStatus::Continue;
  }

  if (direction_ == TrafficDirection::Inbound) {
    auto backward_entry =
        root_->backward_trace_ids_to_headers_.find(b3_span_id_);
    if (backward_entry != root_->backward_trace_ids_to_headers_.end()) {
      auto header_value = backward_entry->second;
      addResponseHeader("x-wasm", header_value);

      LOG_WARN("inbound onResponseHeaders: x-wasm -> " + header_value);
      root_->backward_trace_ids_to_headers_.erase(b3_span_id_);
      root_->forward_trace_ids_to_headers_.erase(b3_span_id_);
    } else {
      auto forward_entry =
          root_->forward_trace_ids_to_headers_.find(b3_span_id_);
      if (forward_entry != root_->forward_trace_ids_to_headers_.end()) {
        auto header_value = forward_entry->second;
        addResponseHeader("x-wasm", header_value);
        LOG_WARN("outbound onResponseHeaders: x-wasm -> " + header_value);
        root_->forward_trace_ids_to_headers_.erase(b3_span_id_);
      } else {
        LOG_WARN("no headers found.");
      }
    }
  } else if (direction_ == TrafficDirection::Outbound) {
    auto header = getResponseHeader("x-wasm");
    if (header->data() != nullptr) {
      auto entry = root_->backward_trace_ids_to_headers_.find(b3_span_id_);
      if (entry == root_->backward_trace_ids_to_headers_.end()) {
        root_->backward_trace_ids_to_headers_[b3_span_id_] = header->toString();
      } else {
        entry->second += "," + header->toString();
      }

      LOG_WARN("outbound onResponseHeaders: " + b3_span_id_ + " -> " +
               root_->backward_trace_ids_to_headers_[b3_span_id_]);
    } else {
      LOG_WARN("outbound onResponseHeaders: x-wasm not found");
    }
  }

  return FilterHeadersStatus::Continue;
}
