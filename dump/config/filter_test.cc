#include <regex>
#include <string>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "gtest/gtest.h"

// To run this test
// $> bazel --noworkspace_rc test :filter_test

TEST(FilterTest, ParseSingleMethod) {
  Config config;

  google::protobuf::util::JsonParseOptions options;
  options.case_insensitive_enum_parsing = true;
  options.ignore_unknown_fields = false;
  google::protobuf::util::JsonStringToMessage(
      "{\"headers\": [{\"name\": \"user-agent\", \"value_regex\": \"curl\"}]}",
      &config, options);

  ASSERT_EQ(config.headers().size(), 1);
  EXPECT_EQ(config.headers(0).name(), "user-agent");
  EXPECT_EQ(config.headers(0).value_regex(), "curl");
}

TEST(FilterTest, ParseMultiple) {
  Config config;

  google::protobuf::util::JsonParseOptions options;
  options.case_insensitive_enum_parsing = true;
  options.ignore_unknown_fields = false;
  google::protobuf::util::JsonStringToMessage(
      "{\"headers\": [ {\"name\": \"content-type\", \"value_regex\": \"text\" "
      "}, {\"name\": \"user-agent\", \"value_regex\": \"curl\"}] }",
      &config, options);

  ASSERT_EQ(config.headers().size(), 2);
  EXPECT_EQ(config.headers(1).name(), "user-agent");
  EXPECT_EQ(config.headers(1).value_regex(), "curl");
}

TEST(FilterText, RegexMatch) {
  ASSERT_TRUE(std::regex_search("curl/7.64.1", std::regex("curl")));
  ASSERT_TRUE(std::regex_search("true", std::regex("true")));
}
