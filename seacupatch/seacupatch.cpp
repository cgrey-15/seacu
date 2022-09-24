#include "seacupatch.h"
#include "parsing.hpp"
#include <vector>
#include <string_view>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <variant>
#include <type_traits>
#include <utility>
#include <span>
#ifdef __cpp_lib_spanstream
#include <spanstream>
#else
#include <sstream>
#endif
#include <queue>

class patcher {
public:
	using character_type = char;
	using buffer_type = std::basic_string<character_type>;
	using source_type = std::string_view;
	using line_type = std::string_view;
	patcher(std::string_view source, size_t preallocate = 0);
	~patcher() = default;

	patcher(const patcher& other);

	patcher& operator=(const patcher& other);

	void insertAdd(ptrdiff_t pos, std::span<const std::string_view> lines);
	void insertDel(ptrdiff_t begin, ptrdiff_t end);
	void insertRepl(ptrdiff_t oldBegin, ptrdiff_t oldEnd, ptrdiff_t pos, character_type const* begin, character_type const* end);
	void flushRest();
	auto lineCount() const noexcept { return srcLines_.size(); }

	bool sourceExhausted() const noexcept { return !(currSrcPos_ < src_.size()); }

	std::string_view stdStringView() const noexcept { return std::string_view{ buffer_.data(), buffer_.size() }; }

private:
	ptrdiff_t currSrcPos_;
	source_type src_;
	std::vector<line_type> srcLines_;
	buffer_type buffer_;
};


struct add_t {
	ptrdiff_t srcLineNo;
	std::vector<std::string_view> lines;
};
struct del_t {
	ptrdiff_t srcFirstLine;
	ptrdiff_t srcLastLine;
};
struct repl_t {
	ptrdiff_t oldFirst;
	ptrdiff_t oldLast;
	ptrdiff_t oldPosition;
	std::vector<std::string_view> newLines;
};

constexpr auto comparator = [](auto&& a, auto&& b) {

	bool res = false;
#if 1
	if (std::holds_alternative<add_t>(a)) {
		auto& val_a = std::get<add_t>(a);
		if (std::holds_alternative<add_t>(b)) {
			auto& val_b = std::get<add_t>(b);
			res = val_a.srcLineNo < val_b.srcLineNo;
		}
		else if (std::holds_alternative<repl_t>(b)) {
			auto& val_b = std::get<repl_t>(b);
			res = val_a.srcLineNo < val_b.oldFirst;
		}
		else {
			auto& val_b = std::get<del_t>(b);
			res = val_a.srcLineNo < val_b.srcFirstLine;
		}
	}
	else if (std::holds_alternative<del_t>(a)) {
		auto& val_a = std::get<del_t>(a);
		if (std::holds_alternative<add_t>(b)) {
			auto& val_b = std::get<add_t>(b);
			res = val_a.srcFirstLine < val_b.srcLineNo;
		}
		else if (std::holds_alternative<repl_t>(b)) {
			auto& val_b = std::get<repl_t>(b);
			res = val_a.srcFirstLine < val_b.oldFirst;
		}
		else {
			auto& val_b = std::get<del_t>(b);
			res = val_a.srcFirstLine < val_b.srcFirstLine;
		}
	}
	else if (std::holds_alternative<repl_t>(a)) {
		auto& val_a = std::get<repl_t>(a);
		if (std::holds_alternative<add_t>(b)) {
			auto& val_b = std::get<add_t>(b);
			res = val_a.oldFirst < val_b.srcLineNo;
		}
		else if (std::holds_alternative<del_t>(b)) {
			res = std::get<repl_t>(a).oldFirst < std::get<del_t>(b).srcFirstLine;
		}
		else {
			res = std::get<repl_t>(a).oldFirst < std::get<repl_t>(b).oldFirst;
		}
	}
	else {
		std::exit(-44);
	}
#endif
	return res;
};

template <typename>
constexpr bool dependent_false_v = false;

using prio_queue_t = std::priority_queue<std::variant<add_t, del_t, repl_t>, std::vector<std::variant<add_t, del_t, repl_t>>, decltype(comparator)>;
using edit_vec_t = std::vector<std::variant<add_t, del_t, repl_t>>;

std::vector<std::string_view> get_lines(std::string_view buf);
std::string apply_changes(std::string_view orig, edit_vec_t& v, tao::pegtl::parse_tree::node& edits);
void processChanges(edit_vec_t& v, patcher& p);

int seacupatch::patch(std::string_view buf_in, std::string_view buf_changes, cmd_opts progOpt) {

	auto in = tao::pegtl::memory_input(buf_changes.data(), buf_changes.size(), "buf_changes");
	std::unique_ptr<tao::pegtl::parse_tree::node> root_hdl;
	tao::pegtl::position pos(0, 0, 0, "");
	try {
		root_hdl = tao::pegtl::parse_tree::parse<seacugrammar::normal_grammar, seacugrammar::seacu_select>(in);
	}
	catch (const tao::pegtl::parse_error& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << e.positions().front() << "\n";
		pos = e.positions().front();
	}
	assert(root_hdl);
	auto& root = *root_hdl;

	//auto str = apply_changes(buf_in, root);
	auto theChanges = edit_vec_t{};//prio_queue_t(comparator);

	std::string patched_str = apply_changes(buf_in, theChanges, root);

	if (!std::is_sorted(theChanges.cbegin(), theChanges.cend(), comparator)) {
		if (progOpt.forceSort) {
			std::stable_sort(theChanges.begin(), theChanges.end(), comparator);
		}
		else {
			std::cerr << "Patch file's edits are out of order. Use --force-sort\n";
			return 1;
		}
	}

	patcher machine{ buf_in };
	processChanges(theChanges, machine);
	std::string_view valueThing = machine.stdStringView();
	std::cout << machine.stdStringView() << "";


	return 0;
}

void handle_del_line(std::string_view orig, edit_vec_t& v, ptrdiff_t first, ptrdiff_t last, tao::pegtl::parse_tree::node& line) {
	v.emplace_back(del_t{ .srcFirstLine = first,.srcLastLine = last });
}
void handle_repl_added_line(std::vector<std::string_view>& lines, tao::pegtl::parse_tree::node& line) {
	lines.push_back(line.string_view());
}
void handle_adds(std::string_view orig, edit_vec_t& v, tao::pegtl::parse_tree::node& edit) {
#ifdef __cpp_lib_spanstream
	std::span<const char> edit_info(edit.string_view().data(), edit.string_view().size());
	std::ispanstream instream{ edit_info };
#else
	std::istringstream instream{ edit.string() };
#endif

	char add_symbol, comma;
	std::ptrdiff_t where = -1;
	size_t dest_start = 0, dest_end = 0;

	instream >> where >> add_symbol >> dest_start >> comma >> dest_end;

	assert(!instream.bad());
	std::vector<std::string_view> lines;
	for (auto& line : edit.children) {
		lines.emplace_back(line->string_view());
	}
	v.emplace_back(add_t{ .srcLineNo = where, .lines = std::move(lines) });
}
void handle_dels(std::string_view orig, edit_vec_t& v, tao::pegtl::parse_tree::node& edit) {
#ifdef __cpp_lib_spanstream
	std::span<const char> edit_info(edit.string_view().data(), edit.string_view().size());
	std::ispanstream instream{ edit_info };
#else
	std::istringstream instream{ edit.string() };
#endif

	char del_symbol, comma;
	size_t where_would = -1;
	ptrdiff_t src_start = 0, src_end = 0;

	instream >> src_start >> comma >> src_end >> del_symbol >> where_would;

	assert(!instream.bad());
	v.emplace_back(del_t{src_start, src_end});
}

void handle_repls(std::string_view orig, edit_vec_t& v, tao::pegtl::parse_tree::node& edit) {
#ifdef __cpp_lib_spanstream
	std::span<const char> edit_info(edit.string_view().data(), edit.string_view().size());
	std::ispanstream instream{ edit_info };
#else
	std::istringstream instream{ edit.string() };
#endif

	char repl_symbol = '\0', comma1, comma2;
	ptrdiff_t old_start = -1, old_end = -1;
	ptrdiff_t new_start = -1, new_last = -1;

	instream >> old_start;
	char tok = instream.get();
	if (tok == ',') {
		instream >> old_end;
		tok = instream.get(); // consume diff symbol now
	}
	else
		old_end = old_start; // tok is diff symbol here
	instream >> new_start;
	tok = instream.get();
	if (tok == ',')
		instream >> new_last;
	else {
		assert(instream.good());
		new_last = new_start;
	}

	std::vector<std::string_view> lines;
	for (auto& line : edit.children.back()->children) {
		handle_repl_added_line(lines, *line);
	}
#ifndef __clang__
	v.emplace_back(repl_t{ old_start, old_end, new_start, std::move(lines) });
#else
	v.emplace_back(repl_t{ old_start, old_end, new_start, std::move(lines) });
#endif
}
void processAdd(add_t info, patcher& p) {
	p.insertAdd(info.srcLineNo, std::span<const std::string_view>(info.lines.data(), info.lines.size()));
}
void processDel(del_t info, patcher& p) {
	p.insertDel(info.srcFirstLine, info.srcLastLine);
}
void processRepl(repl_t info, patcher& p) {
	p.insertDel(info.oldFirst, info.oldLast);

	p.insertAdd(info.oldLast, std::span<const std::string_view>(info.newLines.data(), info.newLines.size()));
}

void processChanges(edit_vec_t& v, patcher& p) {
	auto editCurr = v.cbegin(), editEnd = v.cend();
	while (editCurr != editEnd) {
		std::visit([&p](auto&& arg) {using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, add_t>) {
			add_t arg1 = arg;
			arg1.srcLineNo = std::min<ptrdiff_t>(arg.srcLineNo, p.lineCount());
			processAdd(arg1, p);
		}
		else if constexpr (std::is_same_v<T, del_t>) processDel(arg, p);
		else if constexpr (std::is_same_v<T, repl_t>) processRepl(arg, p);
		else static_assert(dependent_false_v<T>, "Non-exhaustive variant!");
		return; }, *editCurr);

		++editCurr;
	}
	if (!p.sourceExhausted()) {
		p.flushRest();
	}
}

std::string apply_changes(std::string_view orig, edit_vec_t& v, tao::pegtl::parse_tree::node& edit_root)
{
#ifdef __clang__
	constexpr auto addLineSymbol = "seacugrammar::add_line";
	constexpr auto delLineSymbol = "seacugrammar::del_line";
	constexpr auto replLineSymbol = "seacugrammar::repl_line";
#else
	constexpr auto addLineSymbol = "struct seacugrammar::add_line";
	constexpr auto delLineSymbol = "struct seacugrammar::del_line";
	constexpr auto replLineSymbol = "struct seacugrammar::repl_line";
#endif
	std::string ret;
	for (auto& edits : edit_root.children) {
		if (edits->type == addLineSymbol) {
			handle_adds(orig, v, *edits);
		}
		else if (edits->type == delLineSymbol) {
			handle_dels(orig, v, *edits);
		}
		else if (edits->type == replLineSymbol) {
			handle_repls(orig, v, *edits);
		}
	}
	return ret;
}

std::vector<std::string_view> get_lines(std::string_view buf) {

	if (!buf.empty()) {
		assert(buf.back() == '\n');
	}
	std::vector<std::string_view> ret;

	auto curr_it = buf.cbegin(), end = buf.cend();
	auto prev_it = curr_it;

	for (; (curr_it = std::find(curr_it, end, '\n')) != end; prev_it = ++curr_it) {
		ret.emplace_back(prev_it, curr_it + 1);
	}
	return ret;
}
patcher::patcher(std::string_view source, size_t preallocate) : currSrcPos_{}, src_{ source }, srcLines_{get_lines(src_)}, buffer_{}
{
	buffer_.reserve(preallocate);
}
patcher::patcher(const patcher& other)
{
	currSrcPos_ = other.currSrcPos_;
	src_ = other.src_;
	srcLines_ = other.srcLines_;
	buffer_ = other.buffer_;
}
patcher& patcher::operator=(const patcher& other)
{
	currSrcPos_ = other.currSrcPos_;
	src_ = other.src_;
	srcLines_ = other.srcLines_;
	buffer_ = other.buffer_;

	return *this;
}

void patcher::insertAdd(ptrdiff_t lineno, std::span<const std::string_view> lines)
{
	assert(lineno > -1);

	ptrdiff_t consumeAmount;
	character_type const* curr = src_.data() + currSrcPos_;
	if (lineno != srcLines_.size()) {
		assert(lineno < srcLines_.size());
		consumeAmount = srcLines_[lineno].data() - curr;
		character_type const* prefixEnd = curr + consumeAmount;

		buffer_.insert(buffer_.end(), curr, prefixEnd);
	}
	else {
		consumeAmount = src_.end() - (src_.begin() + currSrcPos_);
		buffer_.insert(buffer_.end(), src_.begin() + currSrcPos_, src_.end());
	}
	currSrcPos_ += (consumeAmount);// +(end - begin));
	for (auto line : lines) {
		buffer_.insert(buffer_.end(), line.cbegin(), line.cend());
	}
}

void patcher::insertDel(ptrdiff_t beginLine, ptrdiff_t endLine)
{
	assert(beginLine > 0);
	assert(endLine >= beginLine);

	character_type const* curr = src_.data() + currSrcPos_;

	ptrdiff_t consumeAmount = srcLines_[beginLine-1].data() - curr;

	character_type const* prefixEnd = curr + consumeAmount;

	buffer_.insert(buffer_.end(), curr, prefixEnd);

	character_type const* delBegin = prefixEnd;
	character_type const* delEnd = srcLines_[endLine-1].data() + srcLines_[endLine-1].size();

	currSrcPos_ += consumeAmount + (delEnd - delBegin);
}

void patcher::insertRepl(ptrdiff_t oldBegin, ptrdiff_t oldEnd, ptrdiff_t pos, patcher::character_type const* begin, patcher::character_type const* end) {
	assert(false);
}
void patcher::flushRest()
{
	ptrdiff_t bytesLeft = src_.size() - currSrcPos_;
	buffer_.insert(buffer_.size(), src_.substr(currSrcPos_));
	currSrcPos_ += bytesLeft;
}
