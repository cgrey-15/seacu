#ifndef SEACUDIFF_TYPES_H
#define SEACUDIFF_TYPES_H
#pragma once
#include <string_view>
#include <vector>
#include <span>
#include <boost/unordered_map.hpp>
#include "seacudiff.h"

namespace seacudiff {
	namespace impl {
		class SequenceProcessor;
	}

	struct DiffEntrySeacufiles {
		DiffEntrySeacufiles(std::string_view rawStr, bool isOperandA);
		//std::span<uint32_t> currRegion{sequence.data(), sequence.size()};
	private:
		friend class impl::SequenceProcessor;
		std::string_view rawContents;
		seacudiff::SimpleSequence sequence;
		boost::unordered::unordered_map<std::size_t, SimpleRecord> hTable;
		std::vector<SimpleRecord> seqRecords;
	};
}
#endif // SEACUDIFF_TYPES_H
