#ifndef SAILMCS_SAILMCS_HPP
#define SAILMCS_SAILMCS_HPP

#include <vector>
#include <chrono>
#include <atomic>
#include <string>
#include <sailmcs/IAligner.hpp>

namespace sailmcs {
	class Runner {
		public:
			Runner(
				const std::vector<Graph> &graphs,
				std::chrono::seconds time,
				IAligner &aligner
			);

			virtual void run();
			virtual void stop();
			virtual const Solution &getSolution() const;
			virtual void solutionGraph(Graph &out) const;
			virtual bool isRoot() const;

		protected:
			const std::vector<Graph> *graphs;
			std::chrono::seconds time;
			IAligner *aligner;

			Solution solution;
			std::atomic<bool> stopped; 
	};
}

#endif
