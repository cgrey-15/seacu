#include <vector>
#include <string_view>

namespace seacudiff {
	class UmberSequence {
		std::vector<std::uint32_t> seq_a;
		std::vector<std::uint32_t> seq_b;
	public:

		template<typename It>
		UmberSequence(It beginLA, It endLA, It beginLB, It endLB);
		bool equalHere(uint32_t lOffsetLHS, uint32_t lOffsetRHS);
	};

}

using standard_sv_it = typename std::string_view::const_iterator;
extern template seacudiff::UmberSequence::UmberSequence(standard_sv_it beginLA, standard_sv_it endLA, standard_sv_it beginLB, standard_sv_it endLB);
using standard_s_it = typename std::string::const_iterator;
extern template seacudiff::UmberSequence::UmberSequence(standard_s_it beginLA, standard_s_it endLA, standard_s_it beginLB, standard_s_it endLB);