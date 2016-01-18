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
				const Solution &current,
				const Solution &best
			) = 0;
	};
}
}
#endif
