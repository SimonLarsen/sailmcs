#ifndef SAILMCS_SAILMCS_HPP
#define SAILMCS_SAILMCS_HPP 

#include <cstdlib>
#include <chrono>
#include <atomic>
#include <iostream>
#include <sailmcs/IAligner.hpp>
#include <sailmcs/Solution.hpp>

namespace sailmcs {
	class SailMCS {
		public:
			SailMCS(
				const std::chrono::seconds time,
				IAligner &aligner
			)
			: time(time)
			, aligner(&aligner)
			, stopped(false)
			{ }

			void run() {
				auto start_time = std::chrono::system_clock::now();
				while((std::chrono::system_clock::now() - start_time) < time && !stopped) {
					aligner->step();
				}
			}

			void stop() {
				if(!stopped) {
					stopped = true;
					std::cerr << "SIGINT received. Stopping gracefully." << std::endl;
				}
				else {
					std::cerr << "Stopping immediately." << std::endl;
					exit(EXIT_FAILURE);
				}
			}

			const Solution &getSolution() const {
				return aligner->getSolution();
			}

		private:
			std::chrono::seconds time;
			IAligner *aligner;
			std::atomic<bool> stopped;
	};
}

#endif
