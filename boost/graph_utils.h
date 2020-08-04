#include <map>
#include <memory>
#include <regex>
#include <string_view>

#include "boost/graph/directed_graph.hpp"
#include "boost/graph/vf2_sub_graph_iso.hpp"

struct Node {
  // ID of the node, either specified by user query, or service_name from trace.
  std::string id;
  // Map from property names to values.
  std::map<std::string, std::string> properties;
};

typedef boost::directed_graph<Node> trace_graph_t;

// Binary function object that returns true if the values in item1 (a map)
// in property_map1 are contained in item2 (a map) in property_map2.
template <typename PropertyMapFirst, typename PropertyMapSecond>
struct property_map_subset {

  property_map_subset(const PropertyMapFirst property_map1,
                      const PropertyMapSecond property_map2)
      : m_property_map1(property_map1), m_property_map2(property_map2) {}

  template <typename ItemFirst, typename ItemSecond>
  bool operator()(const ItemFirst item1, const ItemSecond item2) {
    const auto &map1 = get(m_property_map1, item1);
    const auto &map2 = get(m_property_map2, item2);

    for (const auto &pair : map1) {
      if (map2.find(pair.first) == map2.end() ||
          map2.at(pair.first) != pair.second) {
        return false;
      }
    }

    return true;
  }

private:
  const PropertyMapFirst m_property_map1;
  const PropertyMapSecond m_property_map2;
};

// Returns a property_map_subset object that compares the values
// of property_map1 and property_map2.
template <typename PropertyMapFirst, typename PropertyMapSecond>
property_map_subset<PropertyMapFirst, PropertyMapSecond>
make_property_map_subset(const PropertyMapFirst property_map1,
                         const PropertyMapSecond property_map2) {

  return (property_map_subset<PropertyMapFirst, PropertyMapSecond>(
      property_map1, property_map2));
}

std::vector<std::string> str_split(const std::string &str,
                                   const std::string &delim) {
  std::regex re(delim);
  std::sregex_token_iterator it{str.begin(), str.end(), re, -1};
  return {it, {}};
}

// Generate trace graph from a string representing paths
// a-b-c,a-d
// Above means following
// a has directed edge to b
// b has directed edge to c
// a has directed edge to d
std::unique_ptr<trace_graph_t>
generate_trace_graph_from_paths_header(std::string paths_header) {
  return std::make_unique<trace_graph_t>();
}