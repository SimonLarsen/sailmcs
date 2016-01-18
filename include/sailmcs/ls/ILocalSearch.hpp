#ifndef SAILMCS_LS_ILOCALSEARCH_HPP
#define SAILMCS_LS_ILOCALSEARCH_HPP 

#include <vector>
#include <sailmcs/Solution.hpp>
#include <sailmcs/Graph.hpp>

namespace sailmcs {
namespace ls {
	class ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			) = 0;
	};
}
}

#endif
