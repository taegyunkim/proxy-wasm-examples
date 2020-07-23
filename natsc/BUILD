load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")

wasm_cc_binary(
    name = "filter.wasm",
    srcs = [
        "filter.cc",
    ],
    deps = [
        "//third_party/nats-c",
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)
