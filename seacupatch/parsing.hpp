#ifndef PARSING_HPP
#define PARSING_HPP
#include <tao/pegtl.hpp>
#include <type_traits>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace seacugrammar {
	using namespace tao::pegtl;
	struct nonnegative : plus<digit> {};
	struct range : seq<nonnegative, one<','>, nonnegative> {};
	struct maybe_range : seq<nonnegative, opt<one<','>, nonnegative>> {};
	
	struct add_line : seq<nonnegative, one<'a'>, range, eol> {};
	struct del_line : seq<range, one<'d'>, nonnegative, eol> {};
	struct repl_line : seq<maybe_range, one<'c'>, maybe_range, eol> {};

	struct old_line : seq<string<'<', ' '>, until<eol>> {};
	struct new_line : seq<string<'>', ' '>, until<eol>> {};

	struct add_rule : seq<add_line, plus<new_line>> {};
	struct del_rule : seq<del_line, plus<old_line>> {};
	struct repl_rule : seq<repl_line, plus<old_line>, string<'-', '-', '-'>, eol, plus<new_line>> {};

	struct normal_grammar : must<star<sor<add_rule, del_rule, repl_rule>>, eof> {};


	template< typename Rule > struct seacu_select : std::false_type {};

	template<> struct seacu_select<add_line> : std::true_type {};
	template<> struct seacu_select<del_line> : std::true_type {};
	template<> struct seacu_select<repl_line> : std::true_type {};
	template<> struct seacu_select<until<eol>> : std::true_type {};
	template<> struct seacu_select<plus<new_line>> : std::true_type {};
	template<> struct seacu_select<plus<old_line>> : std::true_type {};
	template<> struct seacu_select<add_rule> : std::true_type {
		static void transform(std::unique_ptr<tao::pegtl::parse_tree::node>& n) {
			using NodeHandle = std::remove_reference_t<decltype(n)>;
			n->children.front()->children = std::move(n->children.back()->children);
			NodeHandle held = std::move(n->children.front());
			NodeHandle gone = std::move(n);
			n = std::move(held);
		}
	};
	template<> struct seacu_select<del_rule> : std::true_type {
		static void transform(std::unique_ptr<tao::pegtl::parse_tree::node>& n) {
			using NodeHandle = std::remove_reference_t<decltype(n)>;
			n->children.front()->children = std::move(n->children.back()->children);
			NodeHandle held = std::move(n->children.front());
			NodeHandle gone = std::move(n);
			n = std::move(held);
		}
	};
	template<> struct seacu_select<repl_rule> : std::true_type {
#if 1
		static void transform(std::unique_ptr<tao::pegtl::parse_tree::node>& n) {
			using NodeHandle = std::remove_reference_t<decltype(n)>;
			NodeHandle gone = std::move(n);

			n = std::move(gone->children.front());
			n->children = std::move(gone->children);
			n->children.erase(n->children.begin());
		}
#endif
	};

}
#endif
