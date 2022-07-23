#ifndef DIFF_STANDARD_H
#define DIFF_STANDARD_H
#include "SeacudiffTypes.h"
#include "seacudiff.h"
#include <vector>
#include <string_view>
#include <tuple>
#include <array>

namespace seacudiff {
	namespace impl {
		template<typename SeqT>
		class e_graph {
		public:
			using v_seq = std::span<const SeqT>;
			e_graph(v_seq a, v_seq b) :
				n{ a.size() }, m{ b.size() },
				a_view{ a }, b_view{ b },
				a_begin{ a_view.begin() }, b_begin{ b_view.begin() },
				k{}, edits{} {}

			struct snake_ep_t {
				int x; int y;
				int u; int v;
			};
			auto find_lcs()->std::vector<SeqT>;
			auto get_diff()->std::vector<edit_t<SeqT>>;
			inline size_t get_edit_count() const noexcept { return edits.size(); }
		private:
			static auto middle_snake(v_seq a, v_seq b)->std::pair<int, snake_ep_t>;
			static auto do_find_lcs(v_seq a, v_seq b)->std::vector<SeqT>;
			auto do_get_diff(v_seq a, v_seq b)->std::vector<edit_t<SeqT>>;
			auto do_get_diff_impl(v_seq a, v_seq b)->std::vector<edit_t<SeqT>>;
			edit_t<SeqT> eval_and_add_edit(int d, int k, int prev_k, int x);

			std::size_t n{};
			std::size_t m{};
			v_seq a_view;
			v_seq b_view;
			typename v_seq::iterator a_begin;
			typename v_seq::iterator b_begin;
			int k;
			std::vector<edit_t<SeqT>> edits;
		};

	}

	namespace impl {
		extern template class e_graph<char>;
		using e_graph_s = e_graph<char>;
		extern template class e_graph<uint32_t>;
		using e_graph_hs = e_graph<uint32_t>;
	}
}
#endif // DIFF_STANDARD
