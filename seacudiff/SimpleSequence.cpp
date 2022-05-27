#include "seacudiff.h"
#include "SeacudiffTypes.h"
#include "SequenceProcessor.h"
#include <span>


seacudiff::SimpleSequence::element_type seacudiff::SimpleSequence::operator[](std::size_t i) {
	return sequence[i];
}
