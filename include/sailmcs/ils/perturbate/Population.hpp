#ifndef SAILMCS_ILS_PERTURBATE_POPULATION_HPP
#define SAILMCS_ILS_PERTURBATE_POPULATION_HPP 

#include <random>
#include <vector>
#include <sailmcs/Graph.hpp>
#include <sailmcs/SimilarityMatrix.hpp>
#include <sailmcs/ils/perturbate/IPerturbator.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	class Population : public IPerturbator {
		public:
			Population(
				const std::vector<Graph> &graphs,
				size_t size,
				float min_pheromone
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
			size_t size;
			std::vector<Solution> population;
			float min_pheromone;
			const std::vector<Graph> *graphs;
			std::vector<SimilarityMatrix> pheromones;

			std::minstd_rand rand_gen;
			std::uniform_real_distribution<float> real_dist;
	};
}
}
}

#endif
