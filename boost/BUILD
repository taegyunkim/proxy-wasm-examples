load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

wasm_cc_binary(
    name = "filter.wasm",
    srcs = [
        "filter.cc",
    ],
    deps = [
        ":graph_utils",
        "@boost//:graph",
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)

cc_library(
    name = "str_utils",
    srcs = [
        "str_utils.cc",
    ],
    hdrs = [
        "str_utils.h",
    ],
)

cc_test(
    name = "str_utils_test",
    srcs = [
        "str_utils_test.cc",
    ],
    deps = [
        ":str_utils",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "graph_utils",
    srcs = [
        "graph_utils.cc",
    ],
    hdrs = [
        "graph_utils.h",
    ],
    deps = [
        ":str_utils",
        "@boost//:graph",
    ],
)

cc_test(
    name = "graph_utils_test",
    srcs = [
        "graph_utils_test.cc",
    ],
    deps = [
        ":graph_utils",
        "@gtest",
        "@gtest//:gtest_main",
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
