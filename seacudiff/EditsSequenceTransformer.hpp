#ifndef EDITS_SEQUENCE_TRANSFORMER_HPP
#define EDITS_SEQUENCE_TRANSFORMER_HPP
#include <string_view>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include <iterator>
#include "seacudiff.h"

namespace seacudiff {
	template<class TX, class TY, auto FNC>
	class edits_sequence_transformer {
		//static_assert(std::is_function_v<decltype(FNC)>);
	public:
		template<template<typename, typename ...> class C, typename... Args, typename... Rest>
		auto getSeqEdit(C<edit_t<TX>, Rest...>& in, Args&... args)->std::vector<edit_t<TY>> {
			if constexpr (std::is_same_v<TX, TY>) {
				return std::vector<edit_t<TY>>(std::move(in));
			}
			else {
				std::vector<edit<TY>> v;
				v.reserve(in.capacity());
				FNC(in.cbegin(), in.cend(), std::back_inserter(v), args...);
				return v;
			}
		}
	};

	//template class edits_sequence_transformer<uint32_t, std::string_view>;
	//template std::vector<edit_t<uint32_t>> edits_sequence_transformer<uint32_t, std::string_view>::getSeqEdit<std::vector<uint32_t>>(std::vector<uint32_t>);
}

#endif // EDITS_SEQUENCE_TRANSFORMER_HPP
