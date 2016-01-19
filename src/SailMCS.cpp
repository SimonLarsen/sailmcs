#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <csignal>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tclap/CmdLine.h>
#include <graphio/GraphReader.hpp>
#include <graphio/GraphWriter.hpp>
#include <sailmcs/Graph.hpp>
#include <sailmcs/Solution.hpp>

#include <sailmcs/SailMCS.hpp>
#include <sailmcs/ils/ILS.hpp>
#include <sailmcs/ils/perturbate/Pheromone.hpp>

#include <sailmcs/sa/IAnnealingSchedule.hpp>
#include <sailmcs/sa/Linear.hpp>
#include <sailmcs/sa/Adaptive.hpp>

#include <sailmcs/ls/ILocalSearch.hpp>
#include <sailmcs/ls/FirstImprovement.hpp>
#include <sailmcs/ls/BestImprovement.hpp>
#include <sailmcs/ls/BestLocal.hpp>

using namespace sailmcs;

namespace {
	SailMCS *mcs_ptr;
}

int main(int argc, const char **argv) {
	try {
		TCLAP::CmdLine cmd(
			"sailmcs",
			"A heuristic algorithm for the multiple maximum common subgraph problem.",
			"1.0", "Simon Larsen <simonhffh@gmail.com>."
		);

		TCLAP::ValueArg<size_t> timeArg("t", "time", "Number of seconds to run before terminating.", true, 0, "seconds", cmd);
		TCLAP::ValueArg<std::string> outGraphArg("o", "output-graph", "Writing solution graph to file.", false, "", "path", cmd);
		TCLAP::ValueArg<std::string> outTableArg("O", "output-table", "Write alignment table to file.", false, "", "path", cmd);

		TCLAP::ValueArg<float> evaporationArg("e", "evaporation", "Evaporation rate for pheromones [0,1). Default: 0.1", false, 0.1f, "rate", cmd);
		TCLAP::ValueArg<float> minPheromoneArg("p", "min-pheromone", "Minimum amount of pheromone allowed for any pair. Default: 10", false, 10.0f, "pheromone", cmd);

		TCLAP::ValueArg<std::string> lsArg("L", "local-search", "Local search strategy {first, best, vertex-best}. Default: vertex-best", false, "vertex-best", "strategy", cmd);

		TCLAP::ValueArg<std::string> annealingArg("A", "annealing", "Annealing schedule {linear, adaptive}. Default: linear", false, "linear", "schedule", cmd);
		TCLAP::ValueArg<float> startTemperatureArg("T", "start-temperature", "Starting temperature for linear annealing. Default: 20", false, 20.0f, "temperature", cmd);
		TCLAP::ValueArg<float> temperatureRiseArg("R", "temperature-rise", "Temperature rise rate for adaptive annealing. Default: 5", false, 5.0f, "rate", cmd);

		TCLAP::UnlabeledMultiArg<std::string> filesArg("files", "Graph files.", true, "GRAPHS", cmd);

		// Parse arguments
		cmd.parse(argc, argv);

		std::chrono::seconds time(timeArg.getValue());
		float evaporation = evaporationArg.getValue();
		float min_pheromone = minPheromoneArg.getValue();
		float start_temperature = startTemperatureArg.getValue();
		float temperature_rise = temperatureRiseArg.getValue();
		const std::vector<std::string> &graphFiles = filesArg.getValue();

		if(evaporation < 0.0f || evaporation > 1.0f) throw std::invalid_argument("Evaporation rate must be in [0,1)");
		if(min_pheromone <= 0.0f) throw std::invalid_argument("Min. pheromone level must be > 0.");
		if(start_temperature <= 0.0f) throw std::invalid_argument("Start temperature must be > 0.");
		if(temperature_rise <= 0.0f) throw std::invalid_argument("Temperature rise rate must be > 0.");
		if(graphFiles.size() <= 1) throw std::invalid_argument("Please supply at least two graphs.");

		// Load graph files
		std::vector<Graph> graphs(graphFiles.size(), 0);
		for(size_t i = 0; i < graphFiles.size(); ++i) {
			std::cerr << "Loading graphs (" << i+1 << "/" << graphFiles.size() << ")" << std::endl;
			graphio::readGraph(graphFiles[i], graphs[i]);
			sailmcs::remove_edge_loops(graphs[i]);
		}

		std::sort(graphs.begin(), graphs.end(),
			[&](const Graph &a, const Graph &b) {
				return num_vertices(a) < num_vertices(b);
			}
		);

		// Perturbator instance
		ils::perturbate::Pheromone perturbator(graphs, evaporation, min_pheromone);

		// Annealing schedule
		sa::IAnnealingSchedule *annealing;
		if(annealingArg.getValue() == "linear") annealing = new sa::Linear(start_temperature);
		else if(annealingArg.getValue() == "adaptive") annealing = new sa::Adaptive(start_temperature, temperature_rise);
		else throw std::invalid_argument("Unknown annealing schedule: " + annealingArg.getValue());

		// Local search strategy
		ls::ILocalSearch *ls;
		if(lsArg.getValue() == "first") ls = new ls::FirstImprovement();
		else if(lsArg.getValue() == "best") ls = new ls::BestImprovement();
		else if(lsArg.getValue() == "vertex-best") ls = new ls::BestLocal();
		else throw std::invalid_argument("Unknown local search strategy: " + lsArg.getValue());

		// ILS instance
		ils::ILS ils(graphs, time, *annealing, *ls, perturbator);

		// MCS runner
		SailMCS mcs(time, ils);

		mcs_ptr = &mcs;
		std::signal(SIGINT, [](int signal){ mcs_ptr->stop(); });
		mcs.run();
		std::signal(SIGINT, SIG_DFL);

	} catch(std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}

	return 0;
}
