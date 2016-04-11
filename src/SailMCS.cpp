#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <csignal>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <tclap/CmdLine.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <graphio/GraphReader.hpp>
#include <graphio/GraphWriter.hpp>
#include <sailmcs/Graph.hpp>
#include <sailmcs/Solution.hpp>

#include <sailmcs/SailMCS.hpp>
#include <sailmcs/ils/ILS.hpp>
#include <sailmcs/ils/perturbate/IPerturbator.hpp>
#include <sailmcs/ils/perturbate/Pheromone.hpp>
#include <sailmcs/ils/perturbate/Uniform.hpp>

#include <sailmcs/sa/IAnnealingSchedule.hpp>
#include <sailmcs/sa/Linear.hpp>
#include <sailmcs/sa/Adaptive.hpp>

#include <sailmcs/ls/ILocalSearch.hpp>
#include <sailmcs/ls/First.hpp>
#include <sailmcs/ls/Best.hpp>
#include <sailmcs/ls/VertexBest.hpp>

using namespace sailmcs;

namespace {
	SailMCS *mcs_ptr;
}

std::chrono::seconds get_time(const std::string &str) {
	std::vector<std::string> parts;
	boost::split(parts, str, boost::is_any_of(":"));
	int seconds = 0;
	size_t n = parts.size();
	if(n > 3) {
		throw std::invalid_argument("Invalid time string. Use format [[hh:]mm:]ss.");
	}
	for(size_t i = 0; i < std::min((size_t)3, n); ++i) {
		seconds += boost::lexical_cast<int>(parts[n-1-i]) * std::pow(60, i);
	}
	return std::chrono::seconds(seconds);
}

int main(int argc, const char **argv) {
	try {
		TCLAP::CmdLine cmd(
			"sailmcs",
			"A heuristic algorithm for the multiple maximum common subgraph problem.",
			"1.0", "Simon Larsen <simonhffh@gmail.com>."
		);

		TCLAP::ValueArg<std::string> timeArg("t", "time", "Time to run algorithm before terminating. Format: [[hh:]mm:]ss.", true, "", "time", cmd);
		TCLAP::ValueArg<int> nthreadsArg("", "nthreads", "Number of threads to use.", false, 0, "threads", cmd);
		TCLAP::ValueArg<std::string> outTableArg("o", "output-table", "Write alignment table to file.", false, "", "path", cmd);
		TCLAP::ValueArg<std::string> outGraphArg("g", "output-graph", "Writing solution graph to file.", false, "", "path", cmd);

		TCLAP::ValueArg<std::string> perturbationArg("P", "perturbation", "Perturbation strategy {pheromone, uniform}. Default: pheromone", false, "pheromone", "strategy", cmd);
		TCLAP::ValueArg<std::string> lsArg("L", "local-search", "Local search strategy {first, best, vertex-best}. Default: vertex-best", false, "vertex-best", "strategy", cmd);
		TCLAP::ValueArg<std::string> annealingArg("A", "annealing", "Annealing schedule {linear, adaptive}. Default: adaptive", false, "adaptive", "schedule", cmd);

		TCLAP::ValueArg<float> evaporationArg("e", "evaporation", "Evaporation rate for pheromones [0,1). Default: 0.3", false, 0.3f, "rate", cmd);
		TCLAP::ValueArg<float> minPheromoneArg("p", "min-pheromone", "Minimum amount of pheromone allowed for any pair. Default: 1.0", false, 1.0f, "amount", cmd);
		TCLAP::ValueArg<float> uniformPctArg("U", "uniform-pct", "Number of swaps to perform in uniform perturbation. Given as % of vertex count. Default: 0.1", false, 0.1f, "percent", cmd);

		TCLAP::ValueArg<float> startTemperatureArg("T", "start-temperature", "Starting temperature for linear annealing. Default: 5.0", false, 5.0f, "temperature", cmd);
		TCLAP::ValueArg<float> temperatureRiseArg("R", "temperature-rise", "Temperature rise rate for adaptive annealing. Default: 1.0", false, 1.0f, "rate", cmd);
		TCLAP::ValueArg<int> adaptiveRestartArg("", "adaptive-restart", "Adaptive annealing restart threshold. Default: 20.", false, 20, "iterations", cmd);

		TCLAP::UnlabeledMultiArg<std::string> filesArg("files", "Graph files.", true, "GRAPHS", cmd);

		// Parse arguments
		cmd.parse(argc, argv);

		std::chrono::seconds time(get_time(timeArg.getValue()));
		float evaporation = evaporationArg.getValue();
		float min_pheromone = minPheromoneArg.getValue();
		float start_temperature = startTemperatureArg.getValue();
		float temperature_rise = temperatureRiseArg.getValue();
		int adaptive_restart = adaptiveRestartArg.getValue();
		size_t nthreads = (nthreadsArg.getValue() > 0 ? nthreadsArg.getValue() : omp_get_max_threads());
		const std::vector<std::string> &graphFiles = filesArg.getValue();

		if(evaporation < 0.0f || evaporation > 1.0f) throw std::invalid_argument("Evaporation rate must be in [0,1)");
		if(min_pheromone <= 0.0f) throw std::invalid_argument("Min. pheromone level must be greater than 0.");
		if(start_temperature <= 0.0f) throw std::invalid_argument("Start temperature must be greater than 0.");
		if(temperature_rise <= 0.0f) throw std::invalid_argument("Temperature rise rate must be greater than 0.");
		if(adaptive_restart <= 0) throw std::invalid_argument("Adaptive annealing restart must be greater than 0.");
		if(graphFiles.size() <= 1) throw std::invalid_argument("Please supply at least two graphs.");
		if(nthreads <= 0) throw std::invalid_argument("Number of threads must be greater than 0.");

		// Set number of threads
		std::cerr << "Using " << nthreads << " threads" << std::endl;
		omp_set_num_threads(nthreads);

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
		ils::perturbate::IPerturbator *perturbator;
		if(perturbationArg.getValue() == "pheromone") perturbator = new ils::perturbate::Pheromone(graphs, evaporation, min_pheromone, nthreads);
		else if(perturbationArg.getValue() == "uniform") perturbator = new ils::perturbate::Uniform(uniformPctArg.getValue(), graphs);
		else throw std::invalid_argument("Unknown perturbation strategy: " + perturbationArg.getValue());

		// Annealing schedule
		sa::IAnnealingSchedule *annealing;
		if(annealingArg.getValue() == "linear") annealing = new sa::Linear(start_temperature);
		else if(annealingArg.getValue() == "adaptive") annealing = new sa::Adaptive(start_temperature, temperature_rise, adaptive_restart);
		else throw std::invalid_argument("Unknown annealing schedule: " + annealingArg.getValue());

		// Local search strategy
		ls::ILocalSearch *ls;
		if(lsArg.getValue() == "first") ls = new ls::First();
		else if(lsArg.getValue() == "best") ls = new ls::Best();
		else if(lsArg.getValue() == "vertex-best") ls = new ls::VertexBest();
		else throw std::invalid_argument("Unknown local search strategy: " + lsArg.getValue());

		// ILS instance
		ils::ILS ils(graphs, time, *annealing, *ls, *perturbator);

		// MCS runner
		SailMCS mcs(time, ils);

		// Run algorithm
		mcs_ptr = &mcs;
		std::signal(SIGINT, [](int signal){ mcs_ptr->stop(); });
		mcs.run();
		std::signal(SIGINT, SIG_DFL);

		if(outGraphArg.isSet()) {
			Graph out_graph(0);
			solution_graph(graphs, mcs.getSolution(), out_graph);
			graphio::writeGraph(out_graph, outGraphArg.getValue());
		}

		if(outTableArg.isSet()) {
			write_alignment_file(graphs, mcs.getSolution(), outTableArg.getValue());
		}

	} catch(std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}

	return 0;
}
