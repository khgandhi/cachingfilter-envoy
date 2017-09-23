package(default_visibility = ["//visibility:public"])

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_cc_binary",
    "envoy_cc_test",
)

envoy_cc_binary(
    name = "envoy",
    repository = "@envoy",
    deps = [
        ":cache_filter_config",
        "@envoy//source/exe:envoy_main_entry_lib",
    ],
)

envoy_cc_library(
    name = "cache_constants",
    repository = "@envoy",
    hdrs = ["cache_constants.h"],
    deps = [
        "@envoy//include/envoy/http:header_map_interface",
    ],
)

envoy_cc_library(
    name = "cache_interface",
    repository = "@envoy",
    hdrs = ["cache.h"],
    deps = [
        "@envoy//include/envoy/http:header_map_interface",
    ],
)

envoy_cc_library(
    name = "cache_lib",
    srcs = ["cache_impl.cc"],
    hdrs = ["cache_impl.h"],
    repository = "@envoy",
    deps = [
        ":cache_constants",
        ":cache_interface",
        "@envoy//include/envoy/upstream:cluster_manager_interface",
        "@envoy//source/common/http:message_lib",
        "@envoy//source/common/http:utility_lib",
        "@envoy//source/common/common:enum_to_int",
        "@envoy//source/common/common:logger_lib",
    ],
)

envoy_cc_library(
    name = "cachefilter_lib",
    srcs = ["cache_filter.cc"],
    hdrs = ["cache_filter.h"],
    repository = "@envoy",
    deps = [
	    ":cache_interface",
        ":cache_constants",
        "@envoy//include/envoy/http:filter_interface",
        "@envoy//include/envoy/upstream:cluster_manager_interface",
        "@envoy//include/envoy/buffer:buffer_interface",
        "@envoy//include/envoy/http:codes_interface",
        "@envoy//source/common/common:assert_lib",
        "@envoy//source/common/http:header_map_lib",
        "@envoy//source/common/buffer:buffer_lib",
        "@envoy//source/common/common:logger_lib",
        "@envoy//source/common/common:enum_to_int",
    ],
)

envoy_cc_library(
    name = "cache_filter_config",
    srcs = ["cache_filter_config.cc"],
    hdrs = ["cache_filter_config.h"],
    repository = "@envoy",
    deps = [
        "@envoy//include/envoy/registry",
        "@envoy//include/envoy/server:filter_config_interface",
        ":cachefilter_lib",
        ":cache_lib",
    ],
)

envoy_cc_test(
    name = "cache_filter_test",
    srcs = ["cache_filter_test.cc"],
    repository = "@envoy",
    deps = [
        "@envoy//source/common/http:header_map_lib",
        "@envoy//source/common/http:headers_lib",
        ":cachefilter_lib",
        ":cache_constants",
        ":cache_interface",
        "@envoy//test/mocks/http:http_mocks",
        "@envoy//test/mocks/upstream:upstream_mocks",
        "@envoy//test/test_common:utility_lib",
        "@envoy//source/common/buffer:buffer_lib",
    ],
)

envoy_cc_test(
    name = "cache_impl_test",
    srcs = ["cache_impl_test.cc"],
    deps = [
        "//source/common/http:header_map_lib",
        "//source/common/http:headers_lib",
        ":cache_lib",
        "//source/common/http:message_lib",
        "//test/mocks/http:http_mocks",
        "//test/mocks/upstream:upstream_mocks",
        "//test/test_common:utility_lib",
    ],
)
