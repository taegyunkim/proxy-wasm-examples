load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

config_setting(
    name = "darwin_build",
    values = {"cpu": "darwin"},
)

configure_make(
    name = "protobuf-c",
    autogen = True,
    configure_env_vars = {
        "AR": "",
    },
    configure_in_place = True,
    configure_options = [
        "--disable-dependency-tracking",
    ],
    lib_source = "@protobuf-c//:all",
    shared_libraries =
        select({
            ":darwin_build": [
                "libprotobuf-c.dylib",
            ],
            "//conditions:default": [
                "libprotobuf-c.so",
            ],
        }),
    static_libraries = [
        "libprotobuf-c.a",
    ],
    visibility = ["//visibility:public"],
)
