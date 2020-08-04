#include <algorithm>
#include <regex>
#include <string>
#include <vector>

#include "str_utils.h"

std::vector<std::string>
str_split(const std::string &str, const std::string &delim, bool filter_empty) {
  std::regex re(delim);
  std::sregex_token_iterator it{str.begin(), str.end(), re, -1};

  std::vector<std::string> result{it, {}};
  if (filter_empty) {
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](std::string s) { return s.empty(); }),
                 result.end());
  }

  return result;
}