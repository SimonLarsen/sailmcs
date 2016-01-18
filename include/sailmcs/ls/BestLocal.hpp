#ifndef SAILMCS_LS_BESTLOCAL_HPP
#define SAILMCS_LS_BESTLOCAL_HPP

#include <sailmcs/ls/ILocalSearch.hpp>

namespace sailmcs {
namespace ls {
	class BestLocal : public ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			);
	};
}
}

#endif
