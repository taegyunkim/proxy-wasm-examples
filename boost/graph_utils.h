#include "boost/graph/vf2_sub_graph_iso.hpp"

// Default print_callback
template <typename Graph1, typename Graph2> struct vf2_no_op_callback {

  vf2_no_op_callback(const Graph1 &graph1, const Graph2 &graph2) {}

  template <typename CorrespondenceMap1To2, typename CorrespondenceMap2To1>
  bool operator()(CorrespondenceMap1To2 f, CorrespondenceMap2To1) const {
    return true;
  }
};
