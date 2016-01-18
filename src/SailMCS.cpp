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

#include <sailmcs/ils/ILS.hpp>
#include <sailmcs/ils/perturbate/Pheromone.hpp>
#include <sailmcs/sa/Linear.hpp>
#include <sailmcs/ls/BestLocal.hpp>

using namespace sailmcs;

namespace {
	ils::ILS *ils_ptr;
}

int main(int argc, const char **argv) {
	try {
		TCLAP::CmdLine cmd(
			"sailmcs",
			"A heuristic algorithm for the multiple maximum common subgraph problem.",
			"1.0", "Simon Larsen <simonhffh@gmail.com>"
		);

		TCLAP::ValueArg<size_t> timeArg("t", "time", "Number of seconds to run before terminating.", true, 0, "seconds", cmd);
		TCLAP::ValueArg<std::string> outGraphArg("o", "output-graph", "Writing solution graph to file.", false, "", "path", cmd);
		TCLAP::ValueArg<std::string> outTableArg("O", "output-table", "Write alignment table to file.", false, "", "path", cmd);
		TCLAP::UnlabeledMultiArg<std::string> filesArg("files", "Graph files.", true, "GRAPHS", cmd);

		// Parse arguments
		cmd.parse(argc, argv);

		std::chrono::seconds time(timeArg.getValue());
		const std::vector<std::string> &graphFiles = filesArg.getValue();

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

		sa::Linear annealing(50.0f);
		ls::BestLocal ls;
		ils::perturbate::Pheromone perturbator(graphs, 0.1f, 1.0f);

		ils::ILS ils(graphs, time, annealing, ls, perturbator);

		ils_ptr = &ils;
		std::signal(SIGINT, [](int signal){ ils_ptr->stop(); });
		ils.run();
		std::signal(SIGINT, SIG_DFL);

	} catch(std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}

	return 0;
}
