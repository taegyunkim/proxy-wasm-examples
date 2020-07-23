load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

config_setting(
    name = "darwin_build",
    values = {"cpu": "darwin"},
)

cmake_external(
    name = "nats-c",
    cache_entries =
        select({
            ":darwin_build": {
                "OPENSSL_ROOT_DIR": "$EXT_BUILD_DEPS/openssl/",
                "OPENSSL_INCLUDE_DIR": "$EXT_BUILD_DEPS/openssl/include",
                "OPENSSL_LIB": "$EXT_BUILD_DEPS/openssl/lib",
                "OPENSSLCRYPTO_LIB": "$EXT_BUILD_DEPS/openssl/lib",
                "NATS_PROTOBUF_INCLUDE_DIR": "$EXT_BUILD_DEPS/protobuf-c/include",
                "NATS_PROTOBUF_LIBRARY": "$EXT_BUILD_DEPS/protobuf-c/lib/libprotobuf-c.dylib",
            },
            "//conditions:default": {
                "OPENSSL_ROOT_DIR": "$EXT_BUILD_DEPS/openssl/",
                "OPENSSL_INCLUDE_DIR": "$EXT_BUILD_DEPS/openssl/include",
                "OPENSSL_LIB": "$EXT_BUILD_DEPS/openssl/lib",
                "OPENSSLCRYPTO_LIB": "$EXT_BUILD_DEPS/openssl/lib",
                "NATS_PROTOBUF_INCLUDE_DIR": "$EXT_BUILD_DEPS/protobuf-c/include",
                "NATS_PROTOBUF_LIBRARY": "$EXT_BUILD_DEPS/protobuf-c/lib/libprotobuf-c.so",
            },
        }),
    defines = ["NDEBUG"],
    lib_source = "@nats-c//:all",
    shared_libraries =
        select({
            ":darwin_build": [
                "libnats.dylib",
            ],
            "//conditions:default": [
                "libnats.so",
            ],
        }),
    static_libraries = [
        "libnats_static.a",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/openssl",
        "//third_party/protobuf-c",
    ],
)
