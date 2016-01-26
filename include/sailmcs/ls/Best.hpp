#ifndef SAILMCS_LS_BEST_HPP
#define SAILMCS_LS_BEST_HPP 

#include <sailmcs/ls/ILocalSearch.hpp>

namespace sailmcs {
namespace ls {
	class Best : public ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			);
	};
}
}

#endif
