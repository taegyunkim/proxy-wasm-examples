#include <regex>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(FilterTest, MapUpdate) {
  std::map<std::string, std::string> spans_to_headers;

  spans_to_headers.insert(std::make_pair("a", "1"));
  spans_to_headers.insert(std::make_pair("b", "2"));

  ASSERT_EQ(2, spans_to_headers.size());
  spans_to_headers.at("a") += "0";

  ASSERT_EQ(spans_to_headers.at("a"), "10");

  auto it = spans_to_headers.find("b");
  it->second += "00";

  ASSERT_EQ(spans_to_headers.at("b"), "200");

  it = spans_to_headers.find("c");
  ASSERT_EQ(it, spans_to_headers.end());
  spans_to_headers.insert(std::make_pair("c", "300"));

  ASSERT_EQ(spans_to_headers.at("c"), "300");

  ASSERT_EQ(3, spans_to_headers.size());

  it = spans_to_headers.find("c");
}

TEST(FilterTest, SplitStr) {
  std::string s{"a-b,a-c,a-d"};
  std::regex delimiter(",");
  std::sregex_token_iterator it{s.begin(), s.end(), delimiter, -1};
  std::vector<std::string> words{it, {}};
  EXPECT_THAT(words, testing::ElementsAre("a-b", "a-c", "a-d"));
}