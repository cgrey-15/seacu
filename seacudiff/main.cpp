#include "seacudiff.h"
#include <iostream>
#include <array>
#include <string_view>
#include <tuple>
#include <iostream>
#include <sstream>

auto try_ses_lcs(std::string_view a, std::string_view b, bool do_backwards = false)->std::string;

int main(int argc, char* argv[]) {
    using namespace seacu;

    bool use_backwards = false;

    if (argc > 1 && std::string_view{ argv[1] } != "-b") {
        if (argc > 2) {
            std::cerr << "Too many arguments. ";
        }
        else {
            std::cerr << "Unsupported/invalid argument. ";
        }
        std::cerr << "Use option '-b' for backwards functionality.\n";
        return 1;
    }
    else if (argc == 1) {
    }
    else {
        use_backwards = true;
    }

    std::array<int, 4> boo = { 1, 2, 3, 4 };

    std::string s_a;
    std::string s_b;

    std::vector<std::pair<std::size_t, std::size_t>> pos_len{};
    std::string str_data{};
    std::string str_buf{};
    std::istringstream str_s{};

    while (std::getline(std::cin, s_a) && std::getline(std::cin, s_b)) {
        auto result = try_ses_lcs(s_a, s_b, use_backwards);
        //std::cout << "Result is " << result.first << "-path.\n";
        std::cout << result << "\n";
        str_s = std::istringstream{ s_a };
        while (str_s >> str_buf) {
            pos_len.push_back({ str_data.size(), str_buf.size() + 1 });
            str_data.append(str_buf);
            str_data.push_back('\n');
        }
    }
    std::vector<std::string_view> lines{};
    for (const auto& el : pos_len) {
        lines.emplace_back(str_data.data() + el.first, el.second);
    }
    seacudiff::SimpleSequence seq{ lines.data(), lines.data() + lines.size() };


    int i = 0;
    int j = 0;

    //auto res = find_lcs({}, {}, {});

    //std::cout << "Hello world! Hello '" << res.c << "'!\n";
    return 0;
}
