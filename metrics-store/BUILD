load("@rules_proto//proto:defs.bzl", "proto_library")
load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")

proto_library(
    name = "tcp_metrics_proto",
    srcs = [
        "tcp_metrics.proto",
    ],
    deps = [
        "@com_google_protobuf//:timestamp_proto",
    ],
)

cc_proto_library(
    name = "tcp_metrics_cc_proto",
    deps = [
        "tcp_metrics_proto",
    ],
)

wasm_cc_binary(
    name = "metrics_collector.wasm",
    srcs = [
        "metrics_collector.cc",
    ],
    deps = [
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)

wasm_cc_binary(
    name = "singleton_queue.wasm",
    srcs = [
        "singleton_queue.cc",
    ],
    deps = [
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)
