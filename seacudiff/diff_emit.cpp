#include "diff_emit.h"
#include <ostream>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <format>
#define BOOST_STATIC_STRING_STANDALONE
#include <boost/static_string/static_string.hpp>

int seacudiff::diff_emit_t::foo() {
	return 0;
}

namespace {
	enum class diff_symbol {
		ADD, DEL, REPL
	};
	static constexpr std::size_t BUFF_SIZE = 13;
}

int emit_chunk_header(seacudiff::diff_options& o, diff_symbol sy, std::size_t where_at, std::size_t first, std::size_t last, std::size_t first2=0, std::size_t last2=0) {
	switch (sy) {
	case diff_symbol::ADD:
		(*o.file) << std::format("{}a{},{}\n", where_at, first, last);
		break;
	case diff_symbol::DEL:
		(*o.file) << std::format("{},{}d{}\n", first, last, where_at);
		break;
	case diff_symbol::REPL:
		if (first == last)
			(*o.file) << std::format("{}c", first);
		else
			(*o.file) << std::format("{},{}c", first, last);
		if (first2 == last2)
			(*o.file) << std::format("{}\n", first2);
		else
			(*o.file) << std::format("{},{}\n", first2, last2);
		break;
	}
	return 0;
}
int emit_line(seacudiff::diff_options& o, std::string_view line, std::string_view prefix);

int emit_affected_line(seacudiff::diff_options& o, std::string_view line, diff_symbol sy) {
	//const char
	boost::static_string<::BUFF_SIZE> prefix;
	switch (sy) {
	case ::diff_symbol::ADD:
		prefix.push_back('>');
		break;
	case ::diff_symbol::DEL:
		prefix.push_back('<');
		break;
	default:
		break;
	}
	prefix.push_back(' ');
	return emit_line(o, line, prefix.subview());
}

int emit_line(seacudiff::diff_options& o, std::string_view line, std::string_view prefix) { 
	(*o.file) << prefix << line << "\n";
	return 0;
}

using in_c = std::vector<seacudiff::edit_t<std::uint32_t>>;
using it_t = typename in_c::const_iterator;
using out_c = std::vector<seacudiff::edit_t<std::string_view>>;
constexpr auto fnc = [](it_t begin, it_t end, std::insert_iterator<out_c> out, const seacudiff::DiffEntrySeacufiles& a, const seacudiff::DiffEntrySeacufiles& b) {
};

//TODO simplify somehow maybe
int emit_diff_symbols(seacudiff::diff_options& o, in_c& edits, const seacudiff::DiffEntrySeacufiles& a, const seacudiff::DiffEntrySeacufiles& b) {
	std::vector<in_c::value_type*> consecut_deletes;
	std::vector<in_c::value_type*>::iterator del_begin{}, del_end{};
	diff_symbol s;
	bool flush_deletes = false;
	int res = 0;

	seacudiff::SimpleRecord const* rec_ptr = nullptr;

	for (auto& e : edits) {
		switch (e.type) {
		case in_c::value_type::type_e::Add:
			s = diff_symbol::ADD;
			break;
		case in_c::value_type::type_e::Delete:
			s = diff_symbol::DEL;
			break;
		case in_c::value_type::type_e::Replace:
			s = diff_symbol::REPL;
			break;
		}

		if (s == diff_symbol::DEL) {
			std::size_t first = e.change_pos;
			std::size_t last = e.change_pos + e.value.size() - 1;
			res = emit_chunk_header(o, diff_symbol::DEL, e.pos, first, last);
			for (auto key : e.value) {
				rec_ptr = a.getLineRecord(key);
				res = emit_affected_line(o, rec_ptr->str, s);
			}
		}
		else if (s == diff_symbol::ADD) {
			std::size_t first = e.change_pos;
			std::size_t last = e.change_pos + e.value.size() - 1;
			res = emit_chunk_header(o, diff_symbol::ADD, e.pos, first, last);
			for (auto key : e.value) {
				rec_ptr = b.getLineRecord(key);
				res = emit_affected_line(o, rec_ptr->str, s);
			}
		}
		else if (s == diff_symbol::REPL) {
			std::size_t firstOld = e.pos;
			std::size_t lastOld = e.pos + e.value.size() - 1;
			std::size_t firstNew = e.change_pos;
			std::size_t lastNew = e.change_pos + e.value2.size() - 1;
			res = emit_chunk_header(o, diff_symbol::REPL, e.pos, firstOld, lastOld, firstNew, lastNew);
			for (auto key : e.value) {
				rec_ptr = a.getLineRecord(key);
				res = emit_affected_line(o, rec_ptr->str, diff_symbol::DEL);
			}
			(*o.file) << "---\n";
			for (auto key : e.value2) {
				rec_ptr = b.getLineRecord(key);
				res = emit_affected_line(o, rec_ptr->str, diff_symbol::ADD);
			}
		}

	}
	return res;
}

seacudiff::diff_emit_t::diff_emit_t(const diff_options& o) noexcept : o_{ o } {}

int seacudiff::diff_emit_t::operator()(std::vector<edit_t<uint32_t>>& v, const DiffEntrySeacufiles& seacudf1, const DiffEntrySeacufiles& seacudf2) {
	return emit_diff_symbols(o_, v, seacudf1, seacudf2);
}
