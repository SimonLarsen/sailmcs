#ifndef SAILMCS_SA_ADAPTIVE_HPP
#define SAILMCS_SA_ADAPTIVE_HPP 

#include <limits>
#include <sailmcs/sa/IAnnealingSchedule.hpp>

namespace sailmcs {
namespace sa {
	class Adaptive : public IAnnealingSchedule {
		public:
			Adaptive(
				const float min_temperature,
				const float rise,
				const int restart
			)
			: min_temperature(min_temperature)
			, rise(rise)
			, restart(restart)
			, nonimproved(0)
			{}

			virtual float temperature(
				std::chrono::seconds total_time,
				std::chrono::seconds elapsed_time,
				const Solution &new_solution,
				const Solution &current_solution
			) {
				float temp = min_temperature + rise * std::log(1.0f + nonimproved);
				return std::max(temp, std::numeric_limits<float>::min());
			}

			virtual void update(
				const Solution &new_solution,
				const Solution &current_solution
			) {
				if(new_solution.quality > current_solution.quality) nonimproved = 0;
				else if(new_solution.quality < current_solution.quality) nonimproved++;

				if(nonimproved == restart) nonimproved = 0;
			}

		private:
			float min_temperature;
			float rise;
			int restart;
			int nonimproved;
	};
}
}

#endif
