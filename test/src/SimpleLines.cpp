#include "seacudiff.h"
#include "SeacudiffTypes.h"
#include "diff_standard.h"
#include "SequenceProcessor.h"
#include "diff_emit.h"

#include <iostream>
#include <algorithm>
#include <tuple>

using line_edit_t = seacudiff::edit_t<uint32_t>;
using edit_list_t = std::vector<line_edit_t>;

auto laoTzuTest()->std::tuple<char const*, char const*, edit_list_t> {
	static constexpr const char lao[] =
		"The way the can be told of is not the eternal Way;\n"
		"The name that can be named is not the eternam name.\n"
		"The Nameless is the origin of Heaven and Earth;\n"
		"The Named is the mother of all things.\n"
		"Therefore let there always be non-being,\n"
		"  so we may see their subtlety,\n"
		"And let there always be being,\n"
		"  so we may see their outcome.\n"
		"The two are the same,\n"
		"But after they are produced,\n"
		"  they have different names.\n";

	static char const tzu[] =
		"The Nameless is the origin of Heaven and Earth;\n"
		"The named is the mother of all things.\n"
		"\n"
		"Therefore let there always be non-being,\n"
		"  so we may see their subtlety,\n"
		"And let there always be being,\n"
		"  so we may see their outcome.\n"
		"The two are the same,\n"
		"But after they are produced,\n"
		"  they have different names.\n"
		"They both may be called deep and profound.\n"
		"Deeper and more profound,\n"
		"The door of all subtleties!\n";

	//constexpr std::string_view str{ lao };
	constexpr auto i1 = std::string_view{ lao }.find("The name that can be named");

	edit_list_t edits{};

	return std::make_tuple(lao, tzu, edits);
}

int runTest(char const* a_str, char const* b_str, std::ptrdiff_t expectedAddCount, std::ptrdiff_t expectedDelCount, std::ptrdiff_t expectedReplaceCount);

int SimpleLines(int argc, char* argv[]) {

	{
		auto [lao, tzu, theEdits] = laoTzuTest();
		int retval = runTest(lao, tzu, 1, 1, 1);
		if (retval != 0) return retval;
	}

	char const mip[] =
		"This is Test01.\n"
		"This is Test02.\n"
		"This is Test03.\n"
		"This is Test04.\n"
		"This is Test05.\n";

	char const mop[] =
		"This is Waffle!\n"
		"This is Orange!\n"
		"This is Test01.\n"
		"This is Test02.\n"
		"This is Test05.\n";

	{
		int retval = runTest(mip, mop, 1, 1, 0);
		if (retval != 0) return retval;
	}

	return 0;
}

int runTest(char const* a_str, char const* b_str, std::ptrdiff_t expectedAddCount, std::ptrdiff_t expectedDelCount, std::ptrdiff_t expectedReplaceCount)
{
	seacudiff::DiffEntrySeacufiles a{ a_str, true };
	seacudiff::DiffEntrySeacufiles b{ b_str, false };
	seacudiff::impl::SequenceProcessor s_proc{ a, b };
	s_proc.update();
	s_proc.computeFinalSequence();

	auto a_v = a.seqData();
	auto b_v = b.seqData();

	auto eg = seacudiff::impl::e_graph<uint32_t>{ a_v, b_v };
	auto diff_res = eg.get_diff();

	auto addCount = std::ranges::count_if(diff_res, [](const auto& v) {return v.type == seacudiff::edit_t<std::uint32_t>::type_e::Add; });
	auto delCount = std::ranges::count_if(diff_res, [](const auto& v) {return v.type == seacudiff::edit_t<std::uint32_t>::type_e::Delete; });
	auto replCount = std::ranges::count_if(diff_res, [](const auto& v) {return v.type == line_edit_t::type_e::Replace; });

	if (addCount != expectedAddCount || delCount != expectedDelCount || replCount != expectedReplaceCount) {
		seacudiff::diff_emit_t{ seacudiff::diff_options(std::cout) }(diff_res, a, b);
		return 1;
	}
	else {
		return 0;
	}
}
