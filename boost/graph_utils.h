#include <map>
#include <memory>
#include <regex>
#include <set>
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
trace_graph_t generate_trace_graph_from_paths_header(std::string paths_header) {
  std::vector<std::string> paths = str_split(paths_header, ",");

  std::set<std::string> vertices;
  std::vector<std::pair<std::string, std::string>> edges;
  for (const std::string &path : paths) {
    std::vector<std::string> vertices_vec = str_split(path, "-");
    for (int i = 0; i < vertices_vec.size(); ++i) {
      vertices.insert(vertices_vec[i]);
      if (i + 1 < vertices_vec.size()) {
        edges.push_back(std::make_pair(vertices_vec[i], vertices_vec[i + 1]));
      }
    }
  }

  std::map<std::string, void *> ids_to_vertex_descriptors;

  trace_graph_t graph;
  for (const auto &vertex : vertices) {
    auto v = graph.add_vertex();
    ids_to_vertex_descriptors.insert({vertex, v});
  }

  for (const auto &edge : edges) {
    const auto &src_id = edge.first;
    auto *src_vertex = ids_to_vertex_descriptors[src_id];

    const auto &dst_id = edge.second;
    auto *dst_vertex = ids_to_vertex_descriptors[dst_id];

    graph.add_edge(src_vertex, dst_vertex);
  }

  std::cout << graph.num_edges() << std::endl;

  return graph;
}