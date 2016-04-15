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
				const Solution &new_solution,
				const Solution &current_solution
			) {
				return std::numeric_limits<float>::min();
			}

			virtual void update(
				const Solution &new_solution,
				const Solution &current_solution
			) { }
	};
}
}

#endif
