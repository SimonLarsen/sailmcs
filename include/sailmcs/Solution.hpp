#ifndef SAILMCS_SOLUTION_HPP
#define SAILMCS_SOLUTION_HPP 

#include <random>
#include <algorithm>
#include <fstream>
#include <boost/numeric/ublas/matrix.hpp>
#include <sailmcs/Graph.hpp>

namespace sailmcs {
	class Solution {
		public:
			Solution() : alignment(0, 0), quality(0) { }

			Solution(size_t n, size_t m) : alignment(m, n), quality(0) { }

			//! Vertex mapping
			boost::numeric::ublas::matrix<int, boost::numeric::ublas::row_major> alignment;

			//! Solution quality
			int quality;
	};

	inline void random_solution(const std::vector<Graph> &graphs, Solution &solution) {
		const size_t m = solution.alignment.size1();
		std::minstd_rand rand_gen(std::random_device{}());

		size_t g = 0;
		for(auto it2 = solution.alignment.begin2(); it2 != solution.alignment.end2(); ++it2) {
			std::vector<vertex_descriptor> seq(num_vertices(graphs[g]));
			std::iota(seq.begin(), seq.end(), 0);
			std::shuffle(seq.begin(), seq.end(), rand_gen);
			std::copy(seq.begin(), seq.begin()+m, it2.begin());
			g++;
		}
	}

	inline void write_alignment_file(
		const std::vector<Graph> &graphs,
		const Solution &solution,
		const std::string &filename
	) {
		std::ofstream file(filename);
		if(!file.good()) {
			throw std::runtime_error(std::string("Could not write to file: ") + filename);
		}

		// Write header
		for(size_t i = 0; i < graphs.size(); ++i) {
			auto &g = graphs[i];
			file << g[boost::graph_bundle].label;
			if(i < graphs.size()-1) file << "\t";
		}
		file << "\n";
		
		// Write values
		auto &alignment = solution.alignment;
		for(size_t i = 0; i < alignment.size1(); ++i) {
			for(size_t j = 0; j < alignment.size2(); ++j) {
				auto &g = graphs[j];
				file << g[alignment(i, j)].label;
				if(j < graphs.size()-1) file << "\t";
			}
			file << "\n";
		}
	}

	inline void solution_graph(
		const std::vector<Graph> &graphs,
		const Solution &solution,
		Graph &out
	) {
		size_t m = solution.alignment.size1();
		size_t n = solution.alignment.size2();

		out = Graph(m);

		// Create vertex labels
		for(size_t i = 0; i < m; ++i) {
			std::string label;
			for(size_t g = 0; g < n; ++g) {
				int u = solution.alignment(i, g);
				label.append(graphs[g][u].label);
				if(g < n-1) {
					label.append("-");
				}
			}
			out[i].label = label;
		}

		// Add edges and edge exception attributes
		for(size_t i = 0; i < m; ++i) {
			for(size_t j = i+1; j < m; ++j) {
				int count = 0;
				for(size_t g = 0; g < n; ++g) {
					int u = solution.alignment(i, g);
					int v = solution.alignment(j, g);
					if(is_adjacent(graphs[g], u, v)) {
						count++;
					}
				}

				if(count == (int)n) add_edge(i, j, out);
			}
		}
	}
}

#endif
