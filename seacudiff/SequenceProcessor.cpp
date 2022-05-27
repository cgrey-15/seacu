#include "SequenceProcessor.h"
#include <cassert>
#include <limits>
#include <span>
#include <memory>
#include "SeacudiffTypes.h"

namespace seacudiff {
	impl::SequenceProcessor& impl::SequenceProcessor::add(SimpleRecord r, origin_e side) {
		auto it = s_.find(r.str);
		if (it == s_.cend()) {
			auto [res, success] = s_.emplace(r.str, SequenceRecord{ r.str, collectionSize, 0, 0 });
			assert(success);
			it = res;
			collectionSize++;
		}
		side == origin_e::A ? it->second.countA++ : it->second.countB++;
		return *this;
	}

	impl::SequenceProcessor& impl::SequenceProcessor::addLines(std::string_view* begin, std::string_view* end, bool isOperandA) {
		for (const auto& line : std::span{ begin, end }) {
			auto it = s_.find(line);
			if (it == s_.cend()) {
				auto [res, success] = s_.emplace(line, SequenceRecord{ line, collectionSize, 0, 0 });
				assert(success);
				it = res;
				collectionSize++;
			}
			isOperandA ? it->second.countA++ : it->second.countB++;
		}
		return *this;
	}
}

seacudiff::impl::SequenceProcessor::SequenceProcessor(DiffEntrySeacufiles& a, DiffEntrySeacufiles& b) : 
	s_{}, a_{ std::addressof(a) }, b_{ std::addressof(b) } {
	for (auto [line, _] : a.seqRecords) {
		auto it = s_.find(line);
		if (it == s_.cend()) {
			auto [res, success] = s_.emplace(line, SequenceRecord{ line, collectionSize, 0, 0 });
			assert(success);
			it = res;
			collectionSize++;
		}
		it->second.countA++;
	}
	for (auto [line, _] : b.seqRecords) {
		auto it = s_.find(line);
		if (it == s_.cend()) {
			auto [res, success] = s_.emplace(line, SequenceRecord{ line, collectionSize, 0, 0 });
			assert(success);
			it = res;
			collectionSize++;
		}
		it->second.countB++;
	}

	a.hTable = computeHashTable(a.seqRecords, true);
	b.hTable = computeHashTable(b.seqRecords, false);
}

// stub
void seacudiff::impl::SequenceProcessor::update() {
	return;
}

// just filling both a and b's SimpleSequence member with actual sequence values 
// as determined by SequenceProcessor
void seacudiff::impl::SequenceProcessor::computeFinalSequence() {
	a_->sequence.sequence = dumpSequence(true);
	b_->sequence.sequence = dumpSequence(false);
}

auto seacudiff::impl::SequenceProcessor::computeHashTable(std::vector<SimpleRecord>& recs, bool isOperandA) -> boost::unordered::unordered_map<std::size_t, SimpleRecord> {
	boost::unordered::unordered_map<std::size_t, SimpleRecord> hash_table{};
	auto end = s_.cend();
	for (auto& rec : recs) {
		auto it = s_.find(rec.str);
		assert(it != end);
		SequenceRecord& r = it->second;
		hash_table.emplace(r.id, SimpleRecord{ rec.str, r.id });
	}
	return hash_table;
}

auto seacudiff::impl::SequenceProcessor::dumpSequence(bool isA) -> std::vector<std::uint32_t>
{
	auto* seq = isA ? a_ : b_;
	using ret_t = decltype(seq->sequence.sequence); // sequence.sequence ...
	ret_t res{};

	for (auto& record : seq->seqRecords) {
		auto id = s_.find(record.str)->second.id;
		assert(id <= std::numeric_limits<uint32_t>::max());
		res.emplace_back(id);
		record.id = id;
	}
	return res;
}
