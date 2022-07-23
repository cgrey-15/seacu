#include "seacupatch.h"
#include "parsing.hpp"
#include <vector>
#include <string_view>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <variant>
#include <type_traits>
#ifdef __cpp_lib_spanstream
#include <span>
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

	void insertAdd(ptrdiff_t pos, character_type const* begin, character_type const* end);
	void insertDel(ptrdiff_t begin, ptrdiff_t end);
	void flushRest();

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
	char const *begin, *end;
};
struct del_t {
	ptrdiff_t srcFirstLine;
	ptrdiff_t srcLastLine;
};

constexpr auto comparator = [](auto&& a, auto&& b) {

	bool res = false;
#if 1
	if (std::holds_alternative<add_t>(a)) {
		auto& val_a = std::get<add_t>(a);
		if (std::holds_alternative<add_t>(b)) {
			auto& val_b = std::get<add_t>(b);
			res = val_a.srcLineNo > val_b.srcLineNo;
		}
		else {
			auto& val_b = std::get<del_t>(b);
			res = val_a.srcLineNo > val_b.srcFirstLine;
		}
	}
	else if (std::holds_alternative<del_t>(a)) {
		auto& val_a = std::get<del_t>(a);
		if (std::holds_alternative<add_t>(b)) {
			auto& val_b = std::get<add_t>(b);
			res = val_a.srcFirstLine > val_b.srcLineNo;
		}
		else {
			auto& val_b = std::get<del_t>(b);
			res = val_a.srcFirstLine > val_b.srcFirstLine;
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

using prio_queue_t = std::priority_queue<std::variant<add_t, del_t>, std::vector<std::variant<add_t, del_t>>, decltype(comparator)>;

std::vector<std::string_view> get_lines(std::string_view buf);
std::string apply_changes(std::string_view orig, prio_queue_t& q, tao::pegtl::parse_tree::node& edits);
void processChanges(prio_queue_t& q, patcher& p);

int seacupatch::patch(std::string_view buf_in, std::string_view buf_changes) {

	auto in = tao::pegtl::memory_input(buf_changes.data(), buf_changes.size(), "buf_changes");
	std::unique_ptr<tao::pegtl::parse_tree::node> root_hdl;
	tao::pegtl::position pos(0, 0, 0, "");
	try {
		root_hdl = tao::pegtl::parse_tree::parse<seacugrammar::normal_grammar, seacugrammar::seacu_select>(in);
	}
	catch (const tao::pegtl::parse_error& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << e.positions().front() << "---KABLAM!\n";
		pos = e.positions().front();
	}
	assert(root_hdl);
	auto& root = *root_hdl;

	//auto str = apply_changes(buf_in, root);
	auto theChanges = prio_queue_t(comparator);

	std::string patched_str = apply_changes(buf_in, theChanges, root);

	std::cout << "THE RESULT:\n";

	patcher machine{ buf_in };
	processChanges(theChanges, machine);
	std::cout << machine.stdStringView() << "\n";
#if 0
	while (!theChanges.empty()) {

		std::visit([](auto&& arg) {using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, add_t>) std::cout << "ADD TYPE: " << arg.srcLineNo << "\n";
		else if constexpr (std::is_same_v<T, del_t>) std::cout << "DEL TYPE: " << arg.srcFirstLine << "\n";
		else static_assert(dependent_false_v<T>, "Non-exhaustive variant!");
		return; }, theChanges.top());

		theChanges.pop();
	}
#endif

	return 0;
}
void handle_add_line(std::string_view orig, prio_queue_t& q, ptrdiff_t lineNo, tao::pegtl::parse_tree::node& line) {
	constexpr std::string_view mssg = "\xe3\x81\xa8\xe6\x9b\xb8\xe3\x81\x84\xe3\x81\xa6\xe5\x85\xa5\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\xbb\xe3\x81\x97\xe3\x81\x84\xe3\x81\xa7\xe3\x81\x99\xe3\x80\x82";
	auto begini = line.string_view().data();
	auto endi = line.string_view().data() + line.string_view().size();
	q.push(add_t{ .srcLineNo = lineNo,.begin = begini, .end = endi });
	std::cout << " " << line.string_view() << mssg << "\n";
}
void handle_del_line(std::string_view orig, prio_queue_t& q, ptrdiff_t first, ptrdiff_t last, tao::pegtl::parse_tree::node& line) {
	constexpr std::string_view mssg = "\xe3\x81\xa3\xe3\x81\xa6\xe6\xb6\x88\xe3\x81\x88\xe3\x81\xa6\xe3\x81\xbb\xe3\x81\x97\xe3\x81\x84\xe3\x81\xa7\xe3\x81\x99\xe3\x80\x82";
	//del_t del{ .srcFirstLine = first,.srcLastLine = last };
	q.push(del_t{ .srcFirstLine = first,.srcLastLine = last });
	std::cout << " " << line.string_view() << mssg << "\n";
}
void handle_adds(std::string_view orig, prio_queue_t& q, tao::pegtl::parse_tree::node& edit) {
#ifdef __cpp_lib_spanstream
	std::span<const char> edit_info(edit.string_view().data(), edit.string_view().size());
	std::ispanstream instream{ edit_info };
#else
	std::istringstream instream{ edit.string() };
#endif

	char add_symbol, comma;
	size_t where = -1;
	size_t dest_start = 0, dest_end = 0;

	instream >> where >> add_symbol >> dest_start >> comma >> dest_end;

	std::cout << where << " " << add_symbol << " " << dest_start << "," << dest_end << "!\n";
	assert(!instream.bad());
	for (auto& line : edit.children) {
		handle_add_line(orig, q, where++, *line);
	}
}
void handle_dels(std::string_view orig, prio_queue_t& q, tao::pegtl::parse_tree::node& edit) {
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

	std::cout << src_start << "," << src_end << " " << del_symbol << " " << where_would << "!\n";
	assert(!instream.bad());
	q.push(del_t{src_start, src_end});
#if 0
	for (auto& line : edit.children) {
		handle_del_line(orig, q, src_start, src_end, *line);
	}
#endif
}
void processAdd(add_t info, patcher& p) {
	std::cout << "(add) POS: " << info.srcLineNo << "\n";
	p.insertAdd(info.srcLineNo, info.begin, info.end);
}
void processDel(del_t info, patcher& p) {
	std::cout << "(del) POS: [" << info.srcFirstLine << ", " << info.srcLastLine << "]\n";
	p.insertDel(info.srcFirstLine, info.srcLastLine);
}

void processChanges(prio_queue_t& q, patcher& p) {
	while (!q.empty() && !p.sourceExhausted()) {
		std::visit([&p](auto&& arg) {using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, add_t>) processAdd(arg, p);
		else if constexpr (std::is_same_v<T, del_t>) processDel(arg, p);
		else static_assert(dependent_false_v<T>, "Non-exhaustive variant!");
		return; }, q.top());

		q.pop();
	}
	if (!p.sourceExhausted()) {
		p.flushRest();
	}
}

std::string apply_changes(std::string_view orig, prio_queue_t& q, tao::pegtl::parse_tree::node& edit_root)
{
	std::string ret;
	for (auto& edits : edit_root.children) {
		if (edits->type == "struct seacugrammar::addLine") {
			handle_adds(orig, q, *edits);
		}
		else if (edits->type == "struct seacugrammar::delLine") {
			handle_dels(orig, q, *edits);
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
void patcher::insertAdd(ptrdiff_t lineno, character_type const* begin, character_type const* end)
{
	assert(lineno > -1);
	character_type const* curr = src_.data() + currSrcPos_;
	ptrdiff_t consumeSize = srcLines_[lineno].data() - curr;
	character_type const* prefixEnd = curr + consumeSize;

	buffer_.insert(buffer_.end(), curr, prefixEnd);
	buffer_.insert(buffer_.end(), begin, end);
	currSrcPos_ += (consumeSize + (end-begin));
}
void patcher::insertDel(ptrdiff_t beginLine, ptrdiff_t endLine)
{
	assert(beginLine > 0);
	assert(endLine >= beginLine);

	character_type const* curr = src_.data() + currSrcPos_;
	ptrdiff_t consumeSize = srcLines_[beginLine-1].data() - curr;
	character_type const* prefixEnd = curr + consumeSize;
	buffer_.insert(buffer_.end(), curr, prefixEnd);

	character_type const* delBegin = prefixEnd;
	character_type const* delEnd = srcLines_[endLine].data();

	currSrcPos_ += (delEnd - delBegin);
}
void patcher::flushRest()
{
	ptrdiff_t bytesLeft = src_.size() - currSrcPos_;
	buffer_.insert(buffer_.size(), src_.substr(currSrcPos_));
	currSrcPos_ += bytesLeft;
}
