#include <iostream>
#include "seacudiff.h"
#include "diff_standard.tpp"

using line_edit_t = seacudiff::edit_t<uint32_t>;
using edit_list_t = std::vector<line_edit_t>;

void callFunc() {
	std::cout << "Hello world!" << std::endl;
}

int MergeEdits(int, char* []) {
	const std::array<uint32_t, 6> arr = { 00U, 11U, 22U, 33U, 44U, 55U };
	{
		edit_list_t t01 = { line_edit_t{line_edit_t::type_e::Add, 1, 1} };
		merge_adjacent_changes(t01);
	}
	{
		edit_list_t t02 = { line_edit_t{line_edit_t::type_e::Delete, 1, 1} };
		merge_adjacent_changes(t02);
	}
	{
		edit_list_t t03 = { {line_edit_t::type_e::Add, 2, 2, std::span<const uint32_t>(arr.data(), 2)},
			                {line_edit_t::type_e::Delete, 3, 3, std::span<const uint32_t>(arr.data(), 1)} };
		merge_adjacent_changes(t03);
	}
	{
		edit_list_t t04 = { {line_edit_t::type_e::Delete, 3, 3, std::span<const uint32_t>(arr.data(), 1)},
	                        {line_edit_t::type_e::Add, 2, 2, std::span<const uint32_t>(arr.data(), 2)} };
		merge_adjacent_changes(t04);
	}
	//callFunc();
	return 0;
}
