#ifndef SAILMCS_ILS_ILS_HPP
#define SAILMCS_ILS_ILS_HPP 

#include <vector>
#include <chrono>
#include <sailmcs/Graph.hpp>
#include <sailmcs/IAligner.hpp>
#include <sailmcs/sa/IAnnealingSchedule.hpp>
#include <sailmcs/ls/ILocalSearch.hpp>
#include <sailmcs/ils/perturbate/IPerturbator.hpp>

namespace sailmcs {
namespace ils {
	class ILS : public IAligner {
		public:
			ILS(
				const std::vector<Graph> &graphs,
				const std::chrono::seconds time,
				sa::IAnnealingSchedule &annealing,
				ls::ILocalSearch &ls,
				ils::perturbate::IPerturbator &perturbator
			);

			virtual void step();
			virtual const Solution &getSolution() const;

		private:
			void construct();
			void evaluate();

			size_t m, n;
			const std::vector<Graph> *graphs;
			std::chrono::seconds time;
			sa::IAnnealingSchedule *annealing;
			ls::ILocalSearch *ls;
			ils::perturbate::IPerturbator *perturbator;

			std::chrono::time_point<std::chrono::system_clock> start_time;
			Solution new_solution;
			Solution current_solution;
			Solution best_solution;

			std::random_device rd;
			std::minstd_rand rand_gen;
			std::uniform_real_distribution<float> real_dist;
	};
}
}

#endif
