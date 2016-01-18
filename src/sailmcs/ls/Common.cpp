#include <sailmcs/ls/Common.hpp>

#include <sailmcs/utility/binary_find.hpp>
#include <sailmcs/utility/difference_iterator.hpp>

namespace sailmcs {
namespace ls {
	void create_map(
		const std::vector<Graph> &graphs,
		const Solution &solution,
		std::vector<std::vector<size_t>> &map
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		map.resize(n);

		for(size_t g = 0; g < n; ++g) {
			map[g].resize(num_vertices(graphs[g]));
			std::fill(map[g].begin(), map[g].end(), m);

			for(size_t i = 0; i < m; ++i) {
				int u = solution.alignment(i, g);
				map[g][u] = i;
			}
		}
	}

	void count_edges(
		const std::vector<Graph> &graphs,
		const std::vector<std::vector<size_t>> &map,
		const Solution &solution,
		edge_count_matrix &edges
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		edges.resize(m, m);

		for(auto it = edges.begin1(); it != edges.end1(); ++it) {
			std::fill(it.begin(), it.end(), 0);
		}

		for(size_t g = 0; g < n; ++g) {
			for(auto it = boost::edges(graphs[g]); it.first != it.second; ++it.first) {
				int u = source(*it.first, graphs[g]);
				int v = target(*it.first, graphs[g]);

				size_t i = map[g][u];
				size_t j = map[g][v];

				if(i != j && i != m && j != m) {
					edges(i, j)++;
				}
			}
		}
	}

	void create_neighbor_lists(
		const std::vector<Graph> &graphs,
		const std::vector<std::vector<size_t>> &map,
		const Solution &solution,
		std::vector<neighbor_list> &neighbors
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		neighbors.resize(n-1);

		for(size_t g = 0; g < n-1; ++g) {
			neighbors[g].resize(m);

			for(size_t i = 0; i < m; ++i) {
				int u = solution.alignment(i, g);
				for(auto it = adjacent_vertices(u, graphs[g]); it.first != it.second; ++it.first) {
					int v = *it.first;
					size_t j = map[g][v];

					if(j != m) {
						neighbors[g][i].push_back(j);
					}
				}
				std::sort(neighbors[g][i].begin(), neighbors[g][i].end());
			}
		}
	}

	int get_delta(
		size_t i,
		size_t j,
		size_t g,
		const std::vector<neighbor_list> &neighbors,
		const edge_count_matrix &edges
	) {

		int delta = 0;

		// Find all nodes connected to u not connected to v
		auto it = make_difference_range<vertex_descriptor, std::vector<vertex_descriptor>::const_iterator>(
			neighbors[g][i].begin(), neighbors[g][i].end(),
			neighbors[g][j].begin(), neighbors[g][j].end()
		);
		std::for_each(it.first, it.second, [&](const vertex_descriptor &l) {
			if(l != j) {
				delta -= 2*edges(i, l) - 1;
				delta += 2*edges(j, l) + 1;
			}
		});

		// Find all nodes connected to v not connected to u
		it = make_difference_range<vertex_descriptor, std::vector<vertex_descriptor>::const_iterator>(
			neighbors[g][j].begin(), neighbors[g][j].end(),
			neighbors[g][i].begin(), neighbors[g][i].end()
		);
		std::for_each(it.first, it.second, [&](const vertex_descriptor &l) {
			if(l != i) {
				delta -= 2*edges(j, l) - 1;
				delta += 2*edges(i, l) + 1;
			}
		});

		return delta;
	}

	void swap(
		size_t i,
		size_t j,
		size_t g,
		int iteration,
		std::vector<neighbor_list> &neighbors,
		edge_count_matrix &edges,
		Solution &solution,
		std::vector<int> &active
	) {
		size_t m = solution.alignment.size1();

		// Temp vector for storing adj. list unions
		std::vector<size_t> union_tmp(m);

		// Update edge counts for swap
		auto it = make_difference_range<vertex_descriptor, std::vector<vertex_descriptor>::const_iterator>(
			neighbors[g][i].begin(), neighbors[g][i].end(),
			neighbors[g][j].begin(), neighbors[g][j].end()
		);
		std::for_each(it.first, it.second, [&](const vertex_descriptor &l) {
			if(l != j) {
				edges(i, l)--;
				edges(j, l)++;
			}
		});

		it = make_difference_range<vertex_descriptor, std::vector<vertex_descriptor>::const_iterator>(
			neighbors[g][j].begin(), neighbors[g][j].end(),
			neighbors[g][i].begin(), neighbors[g][i].end()
		);
		std::for_each(it.first, it.second, [&](const vertex_descriptor &l) {
			if(l != i) {
				edges(j, l)--;
				edges(i, l)++;
			}
		});

		// Find symmetric difference of vertices adjacent to u and v
		// and flip i <-> j in neighbor lists
		auto union_end = std::set_symmetric_difference(
			neighbors[g][i].begin(), neighbors[g][i].end(),
			neighbors[g][j].begin(), neighbors[g][j].end(),
			union_tmp.begin()
		);

		std::for_each(
			union_tmp.begin(),
			union_end,
			[&](const size_t &l) {
				auto it_i = binary_find(neighbors[g][l].begin(), neighbors[g][l].end(), i);
				auto it_j = binary_find(neighbors[g][l].begin(), neighbors[g][l].end(), j);

				if(it_i != neighbors[g][l].end()) {
					*it_i = j;
					fix_order_up(neighbors[g][l].begin(), neighbors[g][l].end(), it_i);
				}
				else if(it_j != neighbors[g][l].end()) {
					*it_j = i;
					fix_order_down(neighbors[g][l].begin(), neighbors[g][l].end(), it_j);
				}

				active[l] = iteration+1;
			}
		);
		active[i] = iteration+1;
		active[j] = iteration+1;

		// Swap alignments and neighbors
		std::swap(solution.alignment(i, g), solution.alignment(j, g));
		std::swap(neighbors[g][i], neighbors[g][j]);
	}

	void finalize(
		const edge_count_matrix &edges,
		Solution &solution
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		int sum = 0;
		#pragma omp parallel for reduction(+:sum)
		for(size_t i = 0; i < m; ++i) {
			for(size_t j = i+1; j < m; ++j) {
				if(edges(i, j) == (int)n) {
					sum++;
				}
			}
		}
		solution.quality = sum;
	}
}
}
