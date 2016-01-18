#ifndef SAILMCS_LS_FIRSTIMPROVEMENT_HPP
#define SAILMCS_LS_FIRSTIMPROVEMENT_HPP 

#include <sailmcs/ls/ILocalSearch.hpp>

namespace sailmcs {
namespace ls {
	class FirstImprovement : public ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			);
	};
}
}

#endif
