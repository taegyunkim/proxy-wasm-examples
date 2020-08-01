load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")

wasm_cc_binary(
    name = "filter.wasm",
    srcs = [
        "filter.cc",
    ],
    deps = [
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)

cc_binary(
    name = "test_boost_graph",
    srcs = [
        "test_boost_graph.cpp",
    ],
    deps = [
        "@boost//:graph",
    ],
)
