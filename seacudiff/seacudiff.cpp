#include "seacudiff.h"
#include <string_view>
#include <tuple>
#include <array>
#include <vector>
#include <iterator>
#include <cassert>
#include <limits>
#include "diff_standard.h"
#include "SeacudiffTypes.h"
#include "SequenceProcessor.h"

template<typename CharT>
static bool is_newline(CharT c) { return c == '\n'; }

int do_in_mem_diff(std::string_view buf_a, std::string_view buf_b, bool lcs_included) {
	seacudiff::DiffEntrySeacufiles a{ buf_a, true};
	seacudiff::DiffEntrySeacufiles b{ buf_b, false};
	seacudiff::impl::SequenceProcessor s_proc{a, b};
	s_proc.update();
	s_proc.computeFinalSequence();
	return 0;
}

seacudiff::DiffEntrySeacufiles::DiffEntrySeacufiles(std::string_view rawStr, bool isOperandA):
	sequence{}, rawContents{rawStr}
{
	std::vector<std::string_view> lines;

	for (auto it = std::cbegin(rawStr),
		end = std::cend(rawStr),
		prev_it = it; it != end;)
	{
		prev_it = it;
		it = std::find_if(it, end, [](decltype(rawStr)::value_type c) {return is_newline(c); });
		assert(it != end);

		lines.emplace_back(prev_it, it);
		seqRecords.emplace_back(SimpleRecord{ std::string_view(prev_it, it), std::numeric_limits<std::size_t>::max() });
		++it;
	}
}

auto try_ses_lcs(std::string_view a, std::string_view b, bool do_backwards=false)->std::pair<std::string, size_t> {
	auto eg = seacudiff::impl::e_graph{ a, b };
	auto str = eg.find_lcs();
	return { std::move(str), eg.get_edit_count() };
}

auto try_ses_diff(std::string_view a, std::string_view b)->std::pair<std::vector<seacudiff::edit_t>, size_t> {
	auto eg = seacudiff::impl::e_graph{ a, b };
	auto vec = eg.get_diff();
	return { std::move(vec), eg.get_edit_count() };
}