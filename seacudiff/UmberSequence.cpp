#include "UmberSequence.h"

bool seacudiff::UmberSequence::equalHere(std::uint32_t lOffsetLHS, std::uint32_t lOffsetRHS) {
	return seq_a[lOffsetLHS] == seq_b[lOffsetRHS];
}


template<typename It>
seacudiff::UmberSequence::UmberSequence(It beginLA, It endLA, It beginLB, It endLB) {

}

template seacudiff::UmberSequence::UmberSequence(standard_sv_it, standard_sv_it, standard_sv_it, standard_sv_it);
