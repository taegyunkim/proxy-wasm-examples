load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")

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
