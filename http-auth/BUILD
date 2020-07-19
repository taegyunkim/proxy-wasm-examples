load("@rules_proto//proto:defs.bzl", "proto_library")

cc_binary(
    name = "filter.wasm",
    srcs = [
        "filter.cc",
    ],
    additional_linker_inputs = ["@envoy_wasm_api//:jslib"],
    linkopts = [
        "--js-library",
        "external/envoy_wasm_api/proxy_wasm_intrinsics.js",
    ],
    deps = [
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
    ],
)
