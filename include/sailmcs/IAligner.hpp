#ifndef SAILMCS_IALIGNER_HPP
#define SAILMCS_IALIGNER_HPP

#include <sailmcs/Solution.hpp>

namespace sailmcs {
	class IAligner {
		public:
			virtual void step() = 0;
			virtual const Solution &getSolution() const = 0;
	};
}

#endif
