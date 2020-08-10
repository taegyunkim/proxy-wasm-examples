#include <map>
#include <set>
#include <string>
#include <vector>

#include "graph_utils.h"

trace_graph_t generate_trace_graph(
    std::set<std::string> vertices,
    std::vector<std::pair<std::string, std::string>> edges,
    std::map<std::string, std::map<std::vector<std::string>, std::string>>
        ids_to_properties) {
  trace_graph_t graph;

  std::map<std::string, void *> ids_to_vertex_descriptors;

  for (const auto &vertex : vertices) {
    trace_graph_t::vertex_descriptor v;
    if (ids_to_properties.find(vertex) != ids_to_properties.end()) {
      v = graph.add_vertex(Node{vertex, ids_to_properties[vertex]});
    } else {
      v = graph.add_vertex(Node{vertex, {}});
    }
    ids_to_vertex_descriptors.insert({vertex, v});
  }

  for (const auto &edge : edges) {
    const auto &src_id = edge.first;
    auto *src_vertex = ids_to_vertex_descriptors[src_id];

    const auto &dst_id = edge.second;
    auto *dst_vertex = ids_to_vertex_descriptors[dst_id];

    graph.add_edge(src_vertex, dst_vertex);
  }

  return graph;
}

trace_graph_t generate_trace_graph_from_headers(std::string paths_header,
                                                std::string properties_header) {
  std::vector<std::string> paths = str_split(paths_header, ",", true);

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

  std::map<std::string, std::map<std::vector<std::string>, std::string>>
      vertices_to_properties;
  std::vector<std::string> properties = str_split(properties_header, ",", true);
  for (const auto &property : properties) {
    // Given a.x.y.z == 123, the vector will have a, x, y, z, 123
    std::vector<std::string> property_split =
        str_split(property, R"([.]|(==)|(\s+))", /*filter_empty=*/true);
    const auto &vertex_id = property_split.front();
    const auto &value = property_split.back();

    vertices_to_properties[vertex_id].insert(
        {std::vector<std::string>{property_split.begin() + 1,
                                  property_split.end() - 1},
         value});
  }

  return generate_trace_graph(vertices, edges, vertices_to_properties);
}

// Default print_callback
struct vf2_get_first_mapping_callback {

  vf2_get_first_mapping_callback(const trace_graph_t &graph1,
                                 const trace_graph_t &graph2,
                                 std::map<std::string, std::string> *mapping)
      : graph1_(graph1), graph2_(graph2), mapping_(mapping) {}

  template <typename CorrespondenceMap1To2, typename CorrespondenceMap2To1>
  bool operator()(CorrespondenceMap1To2 f, CorrespondenceMap2To1) {

    // Print (sub)graph isomorphism map
    BGL_FORALL_VERTICES_T(v, graph1_, trace_graph_t) {
      std::string src_id = boost::get(&Node::id, graph1_, v);
      std::string dst_id = boost::get(&Node::id, graph2_, boost::get(f, v));

      std::cout << src_id << " " << dst_id << std::endl;

      mapping_->insert({src_id, dst_id});
    }

    // Return false to get first mapping, and stop.
    return false;
  }

private:
  const trace_graph_t &graph1_;
  const trace_graph_t &graph2_;
  std::map<std::string, std::string> *mapping_;
};

std::map<std::string, std::string>
get_sub_graph_mapping(const trace_graph_t &graph_small,
                      const trace_graph_t &graph_large) {

  auto mapping = std::map<std::string, std::string>();
  vf2_get_first_mapping_callback callback(graph_small, graph_large,
                                          &mapping);

  auto vertex_comp =
      make_property_map_subset(boost::get(&Node::properties, graph_small),
                               boost::get(&Node::properties, graph_large));

  bool found =
      boost::vf2_subgraph_iso(graph_small, graph_large, callback,
                              boost::vertex_order_by_mult(graph_small),
                              edges_equivalent(boost::always_equivalent())
                                  .vertices_equivalent(vertex_comp));

  return mapping;
}

const Node *get_node_with_id(const trace_graph_t &g, std::string_view id) {
  boost::graph_traits<trace_graph_t>::vertex_iterator it, end;

  for (boost::tie(it, end) = boost::vertices(g); it != end; ++it) {
    if (g[*it].id == id) {
      return &g[*it];
    }
  }

  return nullptr;
}
