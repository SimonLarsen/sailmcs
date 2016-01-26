#ifndef SAILMCS_LS_VERTEXBEST_HPP
#define SAILMCS_LS_VERTEXBEST_HPP

#include <sailmcs/ls/ILocalSearch.hpp>

namespace sailmcs {
namespace ls {
	class VertexBest : public ILocalSearch {
		public:
			virtual void localSearch(
				const std::vector<Graph> &graphs,
				Solution &solution
			);
	};
}
}

#endif
