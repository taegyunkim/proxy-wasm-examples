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

TEST(GraphUtilsTest, DirectedGraphWithVertexProperties) {
  struct VertexProperties {
    std::map<std::string, std::string> properties;
  };

  typedef boost::directed_graph<VertexProperties> graph_type;

  graph_type graph1;

  VertexProperties v0_properties;
  v0_properties.properties.insert({"workload_name", "productpagev1"});
  auto v0 = graph1.add_vertex(v0_properties);
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

  auto vertex_comp = boost::make_property_map_equivalent(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph2));

  // When called without vertex_property_map
  ASSERT_FALSE(boost::vf2_subgraph_iso(
      graph1, graph2, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp)));

  graph_type graph3;
  v0 = graph3.add_vertex(v0_properties);
  VertexProperties v1_properties;
  v1 = graph3.add_vertex();
  v2 = graph3.add_vertex();
  v3 = graph3.add_vertex();
  graph3.add_edge(v0, v1);
  graph3.add_edge(v1, v2);
  graph3.add_edge(v0, v3);

  auto vertex_comp2 = boost::make_property_map_equivalent(
      boost::get(&VertexProperties::properties, graph1),
      boost::get(&VertexProperties::properties, graph3));

  ASSERT_FALSE(boost::vf2_subgraph_iso(
      graph1, graph3, callback, boost::vertex_order_by_mult(graph1),
      edges_equivalent(boost::always_equivalent())
          .vertices_equivalent(vertex_comp2)));
}