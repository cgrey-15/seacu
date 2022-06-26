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
#include "diff_emit.h"

template<typename CharT>
static bool is_newline(CharT c) { return c == '\n'; }

int do_in_mem_diff(std::string_view buf_a, std::string_view buf_b, std::ostream& s) {
	seacudiff::DiffEntrySeacufiles a{ buf_a, true};
	seacudiff::DiffEntrySeacufiles b{ buf_b, false};
	seacudiff::impl::SequenceProcessor s_proc{a, b};
	s_proc.update();
	s_proc.computeFinalSequence();

	auto a_v = a.seqData();
	auto b_v = b.seqData();

	auto eg = seacudiff::impl::e_graph<uint32_t>{ a_v, b_v };
	auto diff_res = eg.get_diff();

	seacudiff::diff_emit_t{seacudiff::diff_options(s)}(diff_res, a, b);

#if 0
	using edit_t_l = seacudiff::edit_t<std::string_view>;
	std::vector<edit_t_l> edits_s{};

	for (const auto& edit : diff_res) {
		seacudiff::SimpleRecord const* r = nullptr;
		switch (edit.type) {
			using namespace seacudiff;
		case edit_t<uint32_t>::type_e::Delete:
			r = a.getLineRecord(edit.value[0]);
			break;
		case edit_t<uint32_t>::type_e::Add:
			r = b.getLineRecord(edit.value[0]);
			break;
		default:
			break;
		}
		// TODO will using a view to SimpleRecord be safe over time?
		edits_s.emplace_back(seacudiff::edit_t<std::string_view>{static_cast<edit_t_l::type_e>(edit.type), edit.pos, std::span<const std::string_view>( &r->str, 1 )});
	}
#endif
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
seacudiff::SimpleRecord const* seacudiff::DiffEntrySeacufiles::getLineRecord(uint32_t id) const noexcept
{
	SimpleRecord const* r = nullptr; 
	auto it = hTable.find(id);
	assert(it != hTable.cend());
	r = &(hTable.find(id)->second);
	return r;
}

auto seacudiff::DiffEntrySeacufiles::seqData() const noexcept -> std::span<const typename seacudiff::SimpleSequence::element_type> {
	return decltype(seqData()){sequence.sequence.data(), sequence.sequence.size()};
}

auto try_ses_lcs_s(std::string_view a, std::string_view b)->std::pair<std::vector<char>, size_t> {
	auto eg = seacudiff::impl::e_graph_s{ a, b };
	auto str = eg.find_lcs();
	return { std::move(str), eg.get_edit_count() };
}

auto try_ses_diff_s(std::string_view a, std::string_view b)->std::pair<std::vector<seacudiff::edit_t<char>>, size_t> {
	auto eg = seacudiff::impl::e_graph_s{ a, b };
	auto vec = eg.get_diff();
	return { std::move(vec), eg.get_edit_count() };
}

