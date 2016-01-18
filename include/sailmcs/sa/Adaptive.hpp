#ifndef SAILMCS_SA_ADAPTIVE_HPP
#define SAILMCS_SA_ADAPTIVE_HPP 

#include <limits>
#include <sailmcs/sa/IAnnealingSchedule.hpp>

namespace sailmcs {
namespace sa {
	class Adaptive : public IAnnealingSchedule {
		public:
			Adaptive(
				float min_temperature,
				float rise
			)
			: min_temperature(min_temperature)
			, rise(rise)
			, nonimproved(0)
			{}

			virtual float temperature(
				std::chrono::seconds total_time,
				std::chrono::seconds elapsed_time,
				const Solution &current,
				const Solution &best
			) {
				if(current.quality > best.quality) nonimproved = 0;
				else if(current.quality < best.quality) nonimproved++;

				float temp = min_temperature + rise * std::log(1.0f + nonimproved);
				return std::max(temp, std::numeric_limits<float>::min());
			}

		private:
			float min_temperature;
			float rise;
			int nonimproved;
	};
}
}

#endif
