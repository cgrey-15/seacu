#idndef DIFF_HISTOGRAM_H
#define DIFF_HISTOGRAM_H
#pragma once
#include <vector>

namespace seacu {
    namespace priv {
        using my_pog_t = std::vector<char*>;
        using s_t = std::string;
        my_pog_t find_lcs(const s_t& a, const s_t& b, some_edit_t r);
    }
}
#endif // DIFF_HISTOGRAM_H
