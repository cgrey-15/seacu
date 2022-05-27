#ifndef DIFF_STANDARD_H
#define DIFF_STANDARD_H
#include "SeacudiffTypes.h"
#include "seacudiff.h"
#include <vector>
#include <string_view>
#include <gsl/gsl>
#include <tuple>
#include <array>

namespace seacudiff {
	namespace impl {
		class e_graph {
			using v_seq = gsl::span<const char>;
			std::size_t n{};
			std::size_t m{};
			v_seq a_view;
			v_seq b_view;
			v_seq::iterator a_begin;
			v_seq::iterator b_begin;
			int k;
			std::vector<edit_t> edits;
		public:
			//e_graph(std::size_t ns, std::size_t ms) : n{ ns }, m{ ms }, k{} {}
			e_graph(gsl::span<const char> a, gsl::span<const char> b) : 
				n{a.size()},  m{b.size()},
				a_view{a}, b_view{b},
				a_begin{ a_view.begin() }, b_begin{ b_view.begin() },
				k{}, edits{} {}
			
			struct snake_ep_t {
				int x; int y;
				int u; int v;
			};
			auto find_lcs()->std::string;
			auto get_diff()->std::vector<edit_t>;
			inline size_t get_edit_count() const noexcept { return edits.size(); }
		private:
			static auto middle_snake(gsl::span<const char> a, gsl::span<const char> b)->std::pair<int, snake_ep_t>;
			static auto do_find_lcs(v_seq a, v_seq b)->std::string;
			auto do_get_diff(v_seq a, v_seq b)->std::vector<edit_t>;
			edit_t eval_and_add_edit(int d, int k, int prev_k, int x);
		};
	}
}
#endif // DIFF_STANDARD