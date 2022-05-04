#include "seacudiff.h"
#include <string_view>
#include <tuple>
#include <array>
#include <vector>
#include "diff_standard.h"

auto try_ses_lcs(std::string_view a, std::string_view b, bool do_backwards=false)->std::string {
	auto eg = seacudiff::impl::e_graph{ a.size(), b.size()};
	//auto res = eg.ses(a, b, do_backwards);
	//auto something = eg.middle_snake(a, b);
	return eg.find_lcs(a, b);
}