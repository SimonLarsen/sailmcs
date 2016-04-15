#ifndef SAILMCS_SA_IANNEALINGSCHEDULE_HPP
#define SAILMCS_SA_IANNEALINGSCHEDULE_HPP 

#include <chrono>
#include <sailmcs/Solution.hpp>

namespace sailmcs {
namespace sa {
	class IAnnealingSchedule {
		public:
			virtual float temperature(
				std::chrono::seconds total_time,
				std::chrono::seconds elapsed_time,
				const Solution &new_solution,
				const Solution &current_solution
			) = 0;

			virtual void update(
				const Solution &new_solution,
				const Solution &current_solution
			) = 0;
	};
}
}
#endif
