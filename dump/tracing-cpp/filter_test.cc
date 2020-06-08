#include <regex>
#include <string>

#include "filter.pb.h"
#include "google/protobuf/util/json_util.h"
#include "gtest/gtest.h"

TEST(FilterTest, ParseName) {
  Config config;

  google::protobuf::util::JsonParseOptions options;
  options.case_insensitive_enum_parsing = true;
  options.ignore_unknown_fields = false;
  google::protobuf::util::JsonStringToMessage("{'name': 'productpage-v1'}",
                                              &config, options);

  EXPECT_EQ(config.name(), "productpage-v1");
}
