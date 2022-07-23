#ifndef PARSING_HPP
#define PARSING_HPP
#include <tao/pegtl.hpp>
#include <type_traits>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace seacugrammar {
	using namespace tao::pegtl;
	struct nonnegative : plus<digit> {};
	struct range : seq<nonnegative, one<','>, nonnegative> {};
	
	struct addLine : seq<nonnegative, one<'a'>, range, eol> {};
	struct delLine : seq<range, one<'d'>, nonnegative, eol> {};

	struct oldLine : seq<string<'<', ' '>, until<eol>> {};
	struct newLine : seq<string<'>', ' '>, until<eol>> {};

	struct addRule : seq<addLine, plus<newLine>> {};
	struct delRule : seq<delLine, plus<oldLine>> {};

	struct normal_grammar : must<star<sor<addRule, delRule>>, eof> {};


	template< typename Rule > struct seacu_select : std::false_type {};

	template<> struct seacu_select<addLine> : std::true_type {};
	template<> struct seacu_select<delLine> : std::true_type {};
	template<> struct seacu_select<until<eol>> : std::true_type {};
	template<> struct seacu_select<plus<newLine>> : std::true_type {};
	template<> struct seacu_select<plus<oldLine>> : std::true_type {};
	template<> struct seacu_select<addRule> : std::true_type {
		static void transform(std::unique_ptr<tao::pegtl::parse_tree::node>& n) {
			using node_hdl_t = std::remove_reference_t<decltype(n)>;
			n->children.front()->children = std::move(n->children.back()->children);
			node_hdl_t held = std::move(n->children.front());
			node_hdl_t gone = std::move(n);
			n = std::move(held);
		}
	};
	template<> struct seacu_select<delRule> : std::true_type {
		static void transform(std::unique_ptr<tao::pegtl::parse_tree::node>& n) {
			using node_hdl_t = std::remove_reference_t<decltype(n)>;
			n->children.front()->children = std::move(n->children.back()->children);
			node_hdl_t held = std::move(n->children.front());
			node_hdl_t gone = std::move(n);
			n = std::move(held);
		}
	};

}
#endif
