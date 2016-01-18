#include <sailmcs/ls/FirstImprovement.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <sailmcs/ls/Common.hpp>

namespace ublas = boost::numeric::ublas;

namespace sailmcs {
namespace ls {
	void FirstImprovement::localSearch(
		const std::vector<Graph> &graphs,
		Solution &solution
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		// Create reverse alignment mapping
		std::vector<std::vector<size_t>> map;
		create_map(graphs, solution, map);

		// Count number of graphs each edge is covered in
		edge_count_matrix edges;
		count_edges(graphs, map, solution, edges);

		// Build neighbor lists
		std::vector<neighbor_list> neighbors;
		create_neighbor_lists(graphs, map, solution, neighbors);

		// Active index map
		std::vector<int> active(m, 0);

		// Temp vector for storing adj. list unions
		std::vector<size_t> union_tmp(m);

		std::random_device rd;
		std::minstd_rand rand_gen(rd());

		int iteration = 0;
		bool repeat;

		do {
			repeat = false;

			for(size_t g = 0; g < n-1; ++g) {
				for(size_t i = 0; i < m; ++i) {
					for(size_t j = i+1; j < m; ++j) {
						if(active[i] < iteration && active[j] < iteration) continue;

						int delta = get_delta(i, j, g, neighbors, edges);

						if(delta > 0) {
							repeat = true;
							swap(i, j, g, iteration, neighbors, edges, solution, active);
						}
					}
				}
			}

			iteration++;
		} while(repeat);

		// Extract LCS and solution quality
		finalize(edges, solution);
	}
}
}
