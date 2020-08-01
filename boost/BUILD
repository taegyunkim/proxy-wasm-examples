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

cc_binary(
    name = "vf2_sub_graph_iso_example",
    srcs = [
        "vf2_sub_graph_iso_example.cpp",
    ],
    deps = [
        "@boost//:graph",
    ],
)

cc_binary(
    name = "vf2_sub_graph_iso_multi_example",
    srcs = [
        "vf2_sub_graph_iso_multi_example.cpp",
    ],
    deps = [
        "@boost//:graph",
    ],
)
