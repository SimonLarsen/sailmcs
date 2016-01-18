#ifndef SAILMCS_ILS_PERTURBATE_IPERTUBATOR_HPP
#define SAILMCS_ILS_PERTURBATE_IPERTUBATOR_HPP 

#include <sailmcs/Solution.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	class IPerturbator {
		public:
			virtual void perturbate(
				const Solution &current,
				const Solution &best,
				Solution &out
			) = 0;

			virtual void update(
				const Solution &current,
				const Solution &best
			) { }
	};
}
}
}

#endif
