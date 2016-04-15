#ifndef SAILMCS_VERIFIER_HPP
#define SAILMCS_VERIFIER_HPP 

#include <vector>
#include <sailmcs/Solution.hpp>
#include <sailmcs/Graph.hpp>

namespace sailmcs {
	inline void verify(
		const Solution &solution,
		const std::vector<Graph> &graphs
	) {
		const auto &alignment = solution.alignment;
		const size_t n = solution.alignment.size2();
		const size_t m = solution.alignment.size1();

		// Verify alignment
		for(size_t g = 0; g < n; ++g) {
			std::vector<int> count(num_vertices(graphs[g]), 0);

			for(size_t i = 0; i < m; ++i) {
				vertex_descriptor u = alignment(i, g);
				if(u < 0 || u >= num_vertices(graphs[g])) {
					throw std::runtime_error("Invalid node id in alignment: ");
				}
				count[u]++;
				if(count[u] > 1) {
					throw std::runtime_error("Node is aligned more than once.");
				}
			}
		}
	}
}

#endif
