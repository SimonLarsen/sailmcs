#ifndef SAILMCS_LS_COMMON_HPP
#define SAILMCS_LS_COMMON_HPP 

#include <vector>
#include <algorithm>
#include <sailmcs/Solution.hpp>
#include <sailmcs/Graph.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

namespace sailmcs {
namespace ls {
	typedef std::vector<std::vector<vertex_descriptor>> neighbor_list;
	typedef boost::numeric::ublas::symmetric_matrix<
		int,
		boost::numeric::ublas::upper
	> edge_count_matrix;

	template<typename Iter>
	inline void fix_order_down(Iter begin, Iter end, Iter elem) {
		auto value = *elem;
		auto lower = std::lower_bound(begin, end, value);
		std::move_backward(lower, elem, elem+1);
		*lower = value;
	}

	template<typename Iter>
	inline void fix_order_up(Iter begin, Iter end, Iter elem) {
		auto value = *elem;
		auto upper = std::upper_bound(begin, end, value);
		std::move(elem+1, upper, elem);
		*(upper-1) = value;
	}

	void create_map(
		const std::vector<Graph> &graphs,
		const Solution &solution,
		std::vector<std::vector<size_t>> &map
	);

	void count_edges(
		const std::vector<Graph> &graphs,
		const std::vector<std::vector<size_t>> &map,
		const Solution &solution,
		edge_count_matrix &edges
	);

	void create_neighbor_lists(
		const std::vector<Graph> &graphs,
		const std::vector<std::vector<size_t>> &map,
		const Solution &solution,
		std::vector<neighbor_list> &neighbors
	);

	int get_delta(
		size_t i,
		size_t j,
		size_t g,
		const std::vector<neighbor_list> &neighbors,
		const edge_count_matrix &edges
	);

	void swap(
		size_t i,
		size_t j,
		size_t g,
		int iteration,
		std::vector<neighbor_list> &neighbors,
		edge_count_matrix &edges,
		Solution &solution,
		std::vector<int> &active
	);

	void finalize(
		const edge_count_matrix &edges,
		Solution &solution
	);
}
}

#endif
