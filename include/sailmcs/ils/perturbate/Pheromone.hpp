#ifndef SAILMCS_ILS_PERTURBATE_PHEROMONE_HPP
#define SAILMCS_ILS_PERTURBATE_PHEROMONE_HPP 

#include <random>
#include <vector>
#include <sailmcs/Graph.hpp>
#include <sailmcs/SimilarityMatrix.hpp>
#include <sailmcs/ils/perturbate/IPerturbator.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	class Pheromone : public IPerturbator {
		public:
			Pheromone(
				const std::vector<Graph> &graphs,
				const float evaporation,
				const float min_pheromone,
				const size_t nthreads
			);

			virtual void perturbate(
				const Solution &current,
				const Solution &best,
				Solution &out
			);

			virtual void update(
				const Solution &current,
				const Solution &best
			);

		private:
			size_t n;
			float evaporation;
			float min_pheromone;
			const std::vector<Graph> *graphs;
			std::vector<SimilarityMatrix> pheromones;

			std::vector<std::minstd_rand> rand_gen;
			std::vector<std::uniform_real_distribution<float>> real_dist;
	};
}
}
}

#endif
