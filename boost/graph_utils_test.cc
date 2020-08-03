#include <map>

#include "graph_utils.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/directed_graph.hpp"
#include "boost/graph/vf2_sub_graph_iso.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(GraphUtilsTest, DirectedGraph) {
  typedef boost::directed_graph<> graph_type;

  graph_type graph1;
  auto v0 = graph1.add_vertex();
  auto v1 = graph1.add_vertex();
  auto v2 = graph1.add_vertex();
  graph1.add_edge(v2, v0);
  graph1.add_edge(v2, v1);

  graph_type graph2;
  v0 = graph2.add_vertex();
  v1 = graph2.add_vertex();
  v2 = graph2.add_vertex();
  auto v3 = graph2.add_vertex();
  graph2.add_edge(v0, v1);
  graph2.add_edge(v1, v2);
  graph2.add_edge(v0, v3);

  boost::vf2_print_callback<graph_type, graph_type> callback(graph1, graph2);

  ASSERT_TRUE(vf2_subgraph_iso(graph1, graph2, callback));
}

struct VertexProperties {
  std::map<std::string, std::string> properties;
};

// Binary function object that returns true if the values for item1
// in property_map1 and item2 in property_map2 are equivalent.
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

// Returns a property_map_equivalent object that compares the values
// of property_map1 and property_map2.
template <typename PropertyMapFirst, typename PropertyMapSecond>
property_map_subset<PropertyMapFirst, PropertyMapSecond>
make_property_map_subset(const PropertyMapFirst property_map1,
                         const PropertyMapSecond property_map2) {

  return (property_map_subset<PropertyMapFirst, PropertyMapSecond>(
      property_map1, property_map2));
}

TEST(GraphUtilsTest, DirectedGraphWithVertexProperties) {
  typedef boost::directed_graph<VertexProperties> graph_type;

  graph_type graph1;

  VertexProperties v2_properties;
  v2_properties.properties.insert({"workload_name", "productpagev1"});
  auto v0 = graph1.add_vertex();
  auto v1 = graph1.add_vertex();
  auto v2 = graph1.add_vertex(v2_properties);
  graph1.add_edge(v2, v0);
  graph1.add_edge(v2, v1);

  graph_type graph2;
  v0 = graph2.add_vertex();
  v1 = graph2.add_vertex();
  v2 = graph2.add_vertex();
  auto v3 = graph2.add_vertex();
  graph2.add_edge(v0, v1);
  graph2.add_edge(v1, v2);
  graph2.add_edge(v0, v3);

  boost::vf2_print_callback<graph_type, graph_type> callback(graph1, graph2);

  auto vertex_comp = boost::make_property_map_equivalent(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph2));

  // When called without vertex_property_map
  ASSERT_FALSE(boost::vf2_subgraph_iso(
      graph1, graph2, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp)));

  graph_type graph3;
  VertexProperties g3_v0_properties;
  g3_v0_properties.properties.insert({"workload_name", "productpagev1"});

  v0 = graph3.add_vertex(g3_v0_properties);
  v1 = graph3.add_vertex();
  v2 = graph3.add_vertex();
  v3 = graph3.add_vertex();
  graph3.add_edge(v0, v1);
  graph3.add_edge(v1, v2);
  graph3.add_edge(v0, v3);

  auto vertex_comp2 = boost::make_property_map_equivalent(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph3));

  ASSERT_TRUE(boost::vf2_subgraph_iso(
      graph1, graph3, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp2)));
}

TEST(GraphUtilsTest, DirectedGraphPropertySubset) {
  typedef boost::directed_graph<VertexProperties> graph_type;

  graph_type graph1;
  VertexProperties v0_properties;
  v0_properties.properties.insert({"workload_name", "productpagev1"});
  auto v0 = graph1.add_vertex(v0_properties);

  graph_type graph2;
  VertexProperties v1_properties;
  v1_properties.properties.insert({"workload_name", "productpagev1"});
  v1_properties.properties.insert({"id", "abc"});
  auto v1 = graph2.add_vertex(v1_properties);

  auto vertex_comp = boost::make_property_map_equivalent(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph2));

  boost::vf2_print_callback<graph_type, graph_type> callback(graph1, graph2);

  ASSERT_FALSE(boost::vf2_subgraph_iso(
      graph1, graph2, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp)));

  auto vertex_comp2 = make_property_map_subset(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph2));

  ASSERT_TRUE(boost::vf2_subgraph_iso(
      graph1, graph2, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp2)));
}