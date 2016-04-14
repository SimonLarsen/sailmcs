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
		random_solution(graphs, new_solution);
		current_solution = new_solution;
		best_solution = new_solution;
	}

	void ILS::step() {
		construct();
		evaluate();
	}

	const Solution &ILS::getSolution() const {
		return best_solution;
	}

	void ILS::construct() {
		perturbator->perturbate(current_solution, best_solution, new_solution);

		ls->localSearch(*graphs, new_solution);

		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now() - start_time
		);

		float temperature = annealing->temperature(
			time,
			elapsed_time,
			new_solution,
			current_solution
		);

		float q_diff = (float)new_solution.quality - current_solution.quality;

		float prob = std::min(1.0f, std::exp(q_diff / temperature));

		if(real_dist(rand_gen) < prob) {
			current_solution = new_solution;
			if(current_solution.quality > best_solution.quality) {
				best_solution = current_solution;
			}
			perturbator->update(current_solution, best_solution);
		}
	}

	void ILS::evaluate() {
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now() - start_time
		);

		std::cout << elapsed_time.count() << "\t"
		          << (float)best_solution.quality / num_edges((*graphs)[0]) << "\t"
		          << best_solution.quality << "\t"
		          << current_solution.quality << std::endl;
	}
}
}
