#ifndef SAILMCS_SA_NONE_HPP
#define SAILMCS_SA_NONE_HPP 

#include <limits>
#include <sailmcs/sa/IAnnealingSchedule.hpp>

namespace sailmcs {
namespace sa {
	class None : public IAnnealingSchedule {
		public:
			None() { }

			virtual float temperature(
				std::chrono::seconds total_time,
				std::chrono::seconds elapsed_time,
				const Solution &current,
				const Solution &best
			) {
				return std::numeric_limits<float>::min();
			}
	};
}
}

#endif
