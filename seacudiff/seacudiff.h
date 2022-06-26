#ifndef SEACUDIFF_H
#define SEACUDIFF_H
#pragma once
#include <vector>
#include <string>
#include <span>
#include <cstdint>
#include <cstddef>
#include <iosfwd>

namespace seacudiff {

	namespace impl {
		class SequenceProcessor;
	}
	template<typename SeqT>
	struct edit_t {
		enum class type_e: uint8_t {Add=0, Delete, Replace, Null};
		type_e type;
		std::ptrdiff_t pos;// first;
		std::ptrdiff_t change_pos;
		std::span<const SeqT> value;
	};

	struct SimpleRecord {
		std::string_view str;
		std::size_t id;
	};

	class SimpleSequence {
	public:
		using element_type = uint32_t;
		// TODO somehow simplify and properly compartmentalize SimpleSequence, SequenceProcessor, and DiffEntrySeacufiles
		SimpleSequence() = default;

		element_type operator[](std::size_t);
	private:
		friend class impl::SequenceProcessor;
		friend class DiffEntrySeacufiles;
		using line_id_t = uint32_t;
		std::vector<element_type> sequence;
	};
}

auto try_ses_lcs_s(std::string_view a, std::string_view b)->std::pair<std::vector<char>, size_t>;
auto try_ses_diff_s(std::string_view a, std::string_view b)->std::pair<std::vector<seacudiff::edit_t<char>>, size_t>;
int do_in_mem_diff(std::string_view buf_a, std::string_view buf_b, std::ostream& s);

#endif // SEACUDIFF_H
