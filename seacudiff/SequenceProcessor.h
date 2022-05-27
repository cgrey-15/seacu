#ifndef SEQUENCE_PROCESSOR_H
#define SEQUENCE_PROCESSOR_H
#pragma once
#include <string_view>
#include <boost/unordered/unordered_map.hpp>
#include "seacudiff.h"

namespace seacudiff {
	class DiffEntrySeacufiles;
	namespace impl {
		enum class origin_e {A, B};

		class SequenceProcessor {
		public:
			struct SequenceRecord {
				std::string_view src;
				size_t id;
				uint32_t countA;
				uint32_t countB;
			};
			using choice_map_t = boost::unordered::unordered_map<std::string_view, SequenceRecord>;

			SequenceProcessor(DiffEntrySeacufiles& diffee_a, DiffEntrySeacufiles& diffee_b);

			SequenceProcessor& addLines(std::string_view* begin, std::string_view* end, bool isOperandA);
			SequenceProcessor& add(SimpleRecord rec, origin_e side);
			void update();
			void computeFinalSequence();
		private:
			choice_map_t s_;
			std::size_t collectionSize{};
			DiffEntrySeacufiles* a_;
			DiffEntrySeacufiles* b_;

			auto computeHashTable(std::vector<SimpleRecord>& recs, bool isOperandA) -> boost::unordered::unordered_map<std::size_t, SimpleRecord>;
			auto dumpSequence(bool isA)->std::vector<std::uint32_t>;
		};
	}
}

#endif //SEQUENCE_PROCESSOR_H