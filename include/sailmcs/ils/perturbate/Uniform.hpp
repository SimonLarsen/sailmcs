#ifndef SAILMCS_ILS_PERTURBATE_UNIFORM_HPP
#define SAILMCS_ILS_PERTURBATE_UNIFORM_HPP 

#include <random>
#include <sailmcs/ils/perturbate/IPerturbator.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	class Uniform : public IPerturbator {
		public:
			Uniform(
				float pct,
				const std::vector<Graph> &graphs
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
			float pct;
			const std::vector<Graph> *graphs;

			std::minstd_rand rand_gen;
	};
}
}
}

#endif
