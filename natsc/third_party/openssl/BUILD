# See https://github.com/bazelbuild/rules_foreign_cc
load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

config_setting(
    name = "darwin_build",
    values = {"cpu": "darwin"},
)

# See https://wiki.openssl.org/index.php/Compilation_and_Installation
# See https://github.com/bazelbuild/rules_foreign_cc/issues/338
configure_make(
    name = "openssl",
    configure_command = "config",
    configure_env_vars =
        select({
            ":darwin_build": {
                "OSX_DEPLOYMENT_TARGET": "10.14",
                "AR": "",
            },
            "//conditions:default": {},
        }),
    configure_options =
        select({
            ":darwin_build": [
                "shared",
                "ARFLAGS=r",
                "enable-ec_nistp_64_gcc_128",
                "no-ssl2",
                "no-ssl3",
                "no-comp",
            ],
            "//conditions:default": [],
        }),
    #defines = ["NDEBUG"], Don't know how to use -D; NDEBUG seems to be the default anyway
    lib_source = "@openssl//:all",
    shared_libraries =
        select({
            ":darwin_build": [
                "libssl.dylib",
                "libcrypto.dylib",
            ],
            "//conditions:default": [
                "libssl.so",
                "libcrypto.so",
            ],
        }),
    static_libraries = [
        "libssl.a",
        "libcrypto.a",
    ],
    visibility = ["//visibility:public"],
)
