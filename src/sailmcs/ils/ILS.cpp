#include <sailmcs/ils/ILS.hpp>

#include <random>
#include <algorithm>

namespace sailmcs {
namespace ils {
	ILS::ILS(
		const std::vector<Graph> &graphs,
		const std::chrono::seconds time,
		sa::IAnnealingSchedule &annealing,
		ls::ILocalSearch &ls,
		ils::perturbate::IPerturbator &perturbator
	)
	: m(num_vertices(graphs.front()))
	, n(graphs.size())
	, graphs(&graphs)
	, time(time)
	, stopped(false)
	, annealing(&annealing)
	, ls(&ls)
	, perturbator(&perturbator)

	, start_time(std::chrono::system_clock::now())
	, new_solution(n, m)
	, current_solution(n, m)
	, best_solution(n, m)

	, rand_gen(rd())
	, real_dist(0.0f, 1.0f)
	{
		random_solution(graphs, current_solution);
		best_solution = current_solution;
	}

	void ILS::run() {
		auto startTime = std::chrono::system_clock::now();
		while((std::chrono::system_clock::now() - startTime) < time && !stopped) {
			step();
		}
	}

	void ILS::stop() {
		stopped = true;
	}

	void ILS::step() {
		construct();
		evaluate();
	}

	const Solution &ILS::getSolution() const {
		return best_solution;
	}

	void ILS::setSolution(const Solution &solution) {
		best_solution = solution;
	}

	void ILS::construct() {
		// Generate random alignment
		auto align_start_time = std::chrono::system_clock::now();
		perturbator->perturbate(current_solution, best_solution, new_solution);
		auto align_end_time = std::chrono::system_clock::now();

		// Do local search
		auto ls_start_time = std::chrono::system_clock::now();
		ls->localSearch(*graphs, new_solution);
		auto ls_end_time = std::chrono::system_clock::now();

		#if !defined(NDEBUG)
		std::cerr << "build align.: " << std::chrono::duration_cast<std::chrono::milliseconds>(align_end_time-align_start_time).count() << " ms" << std::endl;
		std::cerr << "local search: " << std::chrono::duration_cast<std::chrono::milliseconds>(ls_end_time-ls_start_time).count() << " ms" << std::endl;
		#endif
		(void)align_start_time; (void)align_end_time;
		(void)ls_start_time; (void)ls_end_time;

		// Calculate acceptance prob. for annealing schedule
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now() - start_time
		);

		float temperature = annealing->temperature(
			time,
			elapsed_time,
			new_solution,
			current_solution
		);

		float prob = std::min(
			1.0f,
			std::exp((new_solution.quality-current_solution.quality) / temperature)
		);

		// Replace best solution if accepted
		if(real_dist(rand_gen) < prob) {
			current_solution = new_solution;
			if(current_solution.quality > best_solution.quality) {
				setSolution(current_solution);
				perturbator->update(current_solution, best_solution);
			}
		}
	}

	void ILS::evaluate() {
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now() - start_time
		);

		std::cout << elapsed_time.count() << "\t"
		          << best_solution.quality << "\t"
		          << current_solution.quality << std::endl;
	}
}
}
