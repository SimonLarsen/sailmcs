#include <sailmcs/ls/BestImprovement.hpp>

#include <algorithm>
#include <random>
#include <sailmcs/ls/Common.hpp>

namespace ublas = boost::numeric::ublas;

namespace sailmcs {
namespace ls {
	void BestImprovement::localSearch(
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

		std::random_device rd;
		std::minstd_rand rand_gen(rd());

		int iteration = 0;
		bool repeat;

		do {
			repeat = false;

			for(size_t g = 0; g < n-1; ++g) {
				int best_delta = 0;
				size_t best_i = 0;
				size_t best_j = 0;

				#pragma omp parallel
				{
					int prv_best_delta = 0;
					size_t prv_best_i = 0;
					size_t prv_best_j = 0;

					#pragma omp for schedule(static, 1)
					for(size_t i = 0; i < m; ++i) {
						for(size_t j = i+1; j < m; ++j) {
							if(active[i] < iteration && active[j] < iteration) continue;

							int delta = get_delta(i, j, g, neighbors, edges);

							if(delta > 0) {
								active[i] = iteration+1;
								active[j] = iteration+1;
							}

							if(delta > prv_best_delta) {
								prv_best_delta = delta;
								prv_best_i = i;
								prv_best_j = j;
							}
						}
					}

					#pragma omp critical
					{
						if(prv_best_delta > best_delta) {
							best_delta = prv_best_delta;
							best_i = prv_best_i;
							best_j = prv_best_j;
						}
					}
				}

				if(best_delta > 0) {
					repeat = true;
					swap(best_i, best_j, g, iteration, neighbors, edges, solution, active);
				}
			}

			iteration++;
		} while(repeat);

		// Extract LCS and solution quality
		finalize(edges, solution);
	}
}
}
