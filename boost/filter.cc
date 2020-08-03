// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/vf2_sub_graph_iso.hpp"

#include "proxy_wasm_intrinsics.h"

// Default print_callback
template <typename Graph1, typename Graph2> struct print_callback {

  print_callback(const Graph1 &graph1, const Graph2 &graph2)
      : graph1_(graph1), graph2_(graph2) {}

  template <typename CorrespondenceMap1To2, typename CorrespondenceMap2To1>
  bool operator()(CorrespondenceMap1To2 f, CorrespondenceMap2To1) const {
    using namespace boost;
    // Print (sub)graph isomorphism map
    BGL_FORALL_VERTICES_T(v, graph1_, Graph1)
    logWarn('(' + get(vertex_index_t(), graph1_, v) + ", " +
            get(vertex_index_t(), graph2_, get(f, v)) + ") ");

    return true;
  }

private:
  const Graph1 &graph1_;
  const Graph2 &graph2_;
};

class HttpAuthRootContext : public RootContext {
public:
  explicit HttpAuthRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {

    using namespace boost;

    typedef adjacency_list<setS, vecS, bidirectionalS> graph_type;

    // Build graph1
    int num_vertices1 = 8;
    graph_type graph1(num_vertices1);
    add_edge(0, 6, graph1);
    add_edge(0, 7, graph1);
    add_edge(1, 5, graph1);
    add_edge(1, 7, graph1);
    add_edge(2, 4, graph1);
    add_edge(2, 5, graph1);
    add_edge(2, 6, graph1);
    add_edge(3, 4, graph1);

    // Build graph2
    int num_vertices2 = 9;
    graph_type graph2(num_vertices2);
    add_edge(0, 6, graph2);
    add_edge(0, 8, graph2);
    add_edge(1, 5, graph2);
    add_edge(1, 7, graph2);
    add_edge(2, 4, graph2);
    add_edge(2, 7, graph2);
    add_edge(2, 8, graph2);
    add_edge(3, 4, graph2);
    add_edge(3, 5, graph2);
    add_edge(3, 6, graph2);

    // Create callback to print mappings
    vf2_print_callback<graph_type, graph_type> callback(graph1, graph2);

    // Print out all subgraph isomorphism mappings between graph1 and graph2.
    // Vertices and edges are assumed to be always equivalent.
    vf2_subgraph_iso(graph1, graph2, callback);
  }
};

class HttpAuthContext : public Context {
public:
  explicit HttpAuthContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<HttpAuthRootContext *>(static_cast<void *>(root))) {}

private:
  HttpAuthRootContext *root_;
};
static RegisterContextFactory
    register_HttpAuthContext(CONTEXT_FACTORY(HttpAuthContext),
                             ROOT_FACTORY(HttpAuthRootContext), "http_auth");
