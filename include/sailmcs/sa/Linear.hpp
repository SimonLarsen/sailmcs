#ifndef SAILMCS_SA_LINEAR_HPP
#define SAILMCS_SA_LINEAR_HPP 

#include <limits>
#include <sailmcs/sa/IAnnealingSchedule.hpp>

namespace sailmcs {
namespace sa {
	class Linear : public IAnnealingSchedule {
		public:
			Linear(
				float start_temperature
			)
			: start_temperature(start_temperature)
			{}

			virtual float temperature(
				std::chrono::seconds total_time,
				std::chrono::seconds elapsed_time,
				const Solution &current,
				const Solution &best
			) {
				auto tm = std::chrono::duration_cast<std::chrono::milliseconds>(total_time);
				auto em = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);
				float progress = (float)em.count() / tm.count();
				float temp = start_temperature * (1.0f - progress);
				return std::max(temp, std::numeric_limits<float>::min());
			}

		private:
			float start_temperature;
	};
}
}
#endif
