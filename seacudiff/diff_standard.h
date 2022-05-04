#include <vector>
#include <string_view>
#include <gsl/gsl>
#include <tuple>
#include <array>

namespace seacudiff {
	namespace impl {
		class e_graph {
			std::size_t n{};
			std::size_t m{};
			int k;
		public:
			e_graph(std::size_t ns, std::size_t ms) : n{ ns }, m{ ms }, k{} {}
			struct snake_ep_t {
				int x; int y;
				int u; int v;
			};
			auto middle_snake(gsl::span<const char> a, gsl::span<const char> b)->std::pair<int, snake_ep_t>;
			auto find_lcs(gsl::span<const char> a, gsl::span<const char> b)->std::string;
		};
	}
}