#ifndef SAILMCS_LS_BESTIMPROVEMENT_HPP
#define SAILMCS_LS_BESTIMPROVEMENT_HPP 

#include <sailmcs/ls/ILocalSearch.hpp>

namespace sailmcs {
namespace ls {
	class BestImprovement : public ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			);
	};
}
}

#endif
