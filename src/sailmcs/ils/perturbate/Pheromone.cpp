#include <sailmcs/ils/perturbate/Pheromone.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	Pheromone::Pheromone(
		const std::vector<Graph> &graphs,
		float evaporation,
		float min_pheromone
	)
	: n(graphs.size())
	, evaporation(evaporation)
	, min_pheromone(min_pheromone)
	, graphs(&graphs)
	, pheromones(n-1)
	, rand_gen(std::random_device{}())
	, real_dist(0.0f, 1.0f)
	{
		// Resize pheromones matrices
		for(size_t i = 0; i < n-1; ++i) {
			pheromones[i].resize(num_vertices(graphs[0]), num_vertices(graphs[i+1]), false);
		}
		// Initialize pheromones
		for(auto &m : pheromones) {
			for(auto it1 = m.begin1(); it1 != m.end1(); ++it1) {
				std::fill(it1.begin(), it1.end(), min_pheromone);
			}
		}
	}

	void Pheromone::perturbate(
		const Solution &current,
		const Solution &best,
		Solution &out
	) {
		size_t m = current.alignment.size1();

		// Create unused lists
		std::vector<std::vector<vertex_descriptor>> unused(n);
		for(size_t i = 0; i < n; ++i) {
			unused[i].resize(num_vertices((*graphs)[i]));
			std::iota(unused[i].begin(), unused[i].end(), 0);
		}

		// Fill first column of alignment randomly
		std::iota(out.alignment.begin1(), out.alignment.end1(), 0);
		std::shuffle(out.alignment.begin1(), out.alignment.end1(), rand_gen);

		// Align remaining networks
		size_t start_node;
		for(size_t i = 0; i < m; ++i) {
			start_node = out.alignment(i, 0);

			for(size_t g = 1; g < n; ++g) {
				size_t best_index = 0;
				float best_prob = 0.0f;

				#pragma omp parallel
				{
					size_t prv_best_index = 0;
					float prv_best_prob = 0.0f;

					#pragma omp for schedule(static)
					for(size_t index = 0; index < unused[g].size(); ++index) {
						size_t node = unused[g][index];
						float prob = real_dist(rand_gen) * pheromones[g-1](start_node, node);

						if(prob > prv_best_prob) {
							prv_best_index = index;
							prv_best_prob = prob;
						}
					}

					#pragma omp critical
					{
						if(prv_best_prob > best_prob) {
							best_index = prv_best_index;
							best_prob = prv_best_prob;
						}
					}
				}

				size_t node = unused[g][best_index];
				unused[g].erase(unused[g].begin()+best_index);
				out.alignment(i, g) = node;
			}
		}
	}

	void Pheromone::update(
		const Solution &current,
		const Solution &best
	) {
		size_t m = current.alignment.size1();

		// Evaporate pheromones
		for(auto &m : pheromones) {
			for(auto it1 = m.begin1(); it1 != m.end1(); ++it1) {
				for(auto it = it1.begin(); it != it1.end(); ++it) {
					*it = std::max(*it * (1.0f - evaporation), min_pheromone);
				}
			}
		}

		// Add pheromones for current solution
		float quality = static_cast<float>(current.quality);
		float best_quality = static_cast<float>(best.quality);
		for(size_t i = 0; i < n-1; ++i) {
			for(size_t j = 0; j < m; ++j) {
				int u = current.alignment(j, 0);
				int v = current.alignment(j, i+1);
				pheromones[i](u, v) = std::min(pheromones[i](u,v) + quality, best_quality);
			}
		}
	}
}
}
}
