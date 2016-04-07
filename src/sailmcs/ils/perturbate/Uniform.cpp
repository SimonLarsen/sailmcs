#include <sailmcs/ils/perturbate/Uniform.hpp>

namespace sailmcs {
namespace ils {
namespace perturbate {
	Uniform::Uniform(
		float pct,
		const std::vector<Graph> &graphs
	)
	: pct(pct)
	, graphs(&graphs)
	, rand_gen(std::random_device{}())
	{ }

	void Uniform::perturbate(
		const Solution &current,
		const Solution &best,
		Solution &out
	) {
		const size_t n = current.alignment.size2();
		const size_t m = current.alignment.size1();
		std::uniform_int_distribution<size_t> m_dist(0, m-1);

		out = current;

		for(size_t g = 1; g < n; ++g) {
			const int count = (int)round(pct * num_vertices((*graphs)[g]));

			std::uniform_int_distribution<vertex_descriptor> vertex_dist(0, num_vertices((*graphs)[g])-1);
			std::vector<size_t> map(num_vertices((*graphs)[g]), m);
			for(size_t i = 0; i < m; ++i) {
				int u = out.alignment(i, g);
				map[u] = i;
			}

			for(int c = 0; c < count; ++c) {
				size_t i = m_dist(rand_gen);
				vertex_descriptor u = out.alignment(i, g);
				vertex_descriptor v;

				do {
					v = vertex_dist(rand_gen);
				} while(u == v);

				size_t j = map[v];

				if(j != m) {
					std::swap(out.alignment(i, g), out.alignment(j, g));
				} else {
					out.alignment(i, g) = v;
				}

				std::swap(map[u], map[v]);
			}
		}
	}

	void Uniform::update(const Solution &current, const Solution &best) { }
}
}
}
