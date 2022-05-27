#ifndef SEACUDIFF_H
#define SEACUDIFF_H
#pragma once
#include <vector>
#include <string>

namespace seacu {
	struct some_edit_t {
		char a;
	};

	struct some_type_t {
		char b;
	};

	struct edit_region_t {
		char c;
	};

	edit_region_t find_lcsFOO(some_type_t a, some_type_t b, some_edit_t r);
}
namespace seacudiff {
	namespace impl {
		class SequenceProcessor;
	}
	struct edit_t {
		enum class type_e {Add, Delete, Replace, Null};
		type_e type;
		size_t pos;// first;
		std::string deletedEl;
		std::string addedEl;
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
		using line_id_t = uint32_t;
		std::vector<element_type> sequence;
	};
}

#endif // SEACUDIFF_H
