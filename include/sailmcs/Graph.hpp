#ifndef SAILMCS_GRAPH_HPP
#define SAILMCS_GRAPH_HPP 

#include <string>
#include <utility>
#include <graphio/Graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

namespace sailmcs {
	struct Edge : public graphio::LabeledEdge {};
	struct Vertex : public graphio::LabeledVertex {};
	struct GraphBundle : public graphio::LabeledGraph {};

	typedef boost::adjacency_list<
		boost::setS,
		boost::vecS,
		boost::undirectedS,
		Vertex,
		Edge,
		GraphBundle
	> Graph;

	typedef typename boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;

	typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
	typedef typename boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;

	typedef typename boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;

	typedef std::pair<vertex_descriptor, vertex_descriptor> vertex_pair;

	inline bool is_adjacent(const Graph &g, vertex_descriptor u, vertex_descriptor v) {
		for(auto it = out_edges(u, g); it.first != it.second; ++it.first) {
			if(target(*it.first, g) == v) return true;
		}
		return false;
	}

	inline void remove_edge_loops(Graph &g) {
		remove_edge_if([&g](
			const edge_descriptor &e) {
				return source(e, g) == target(e, g);
			},
			g
		);
	}
}

#endif
