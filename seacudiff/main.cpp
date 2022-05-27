#include "seacudiff.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <array>
#include <string_view>
#include <tuple>
#include <iostream>
#include <sstream>
#include <fstream>

auto try_ses_lcs(std::string_view a, std::string_view b, bool do_backwards = false)->std::pair<std::string, size_t>;
auto try_ses_diff(std::string_view a, std::string_view b)->std::pair<std::vector<seacudiff::edit_t>, size_t>;
int do_char_diff_loop(bool lcs_included);
int do_in_mem_diff(std::string_view buf_a, std::string_view buf_b, bool lcs_included);
int do_file_diff(std::string& filename_a, std::string& filename_b, bool lcs_included);

int main(int argc, char* argv[]) {
    using namespace seacu;

    //static_assert(static_cast<void*>(do_file_diff) == static_cast<void*>(do_in_mem_diff));

    bool use_backwards = false;

    argparse::ArgumentParser prog{ "test_prog", "0.0.5" };

    {
        using namespace std::string_literals;
        prog.add_argument("DiffEntries").default_value(std::vector<std::string>{""s,""s}).nargs(2);
        prog.add_argument("--lcs").implicit_value(true).default_value(false);
    }
    try {
        prog.parse_args(argc, argv);
    }
    catch (std::runtime_error& e) {
        //std::cerr << "oops: " << e.what() << "\n";
        std::cerr << e.what() << "\n";
        std::exit(1);
    }


#if 0
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
        auto huhh = try_ses_diff(s_a, s_b);
        auto& bah = huhh.first;
        //std::cout << "Result is " << result.first << "-path.\n";
        std::cout << result.first << "(E count is " << result.second << ")\n";
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
#endif

    //seacudiff::SimpleSequence seq{ lines.data(), lines.data() + lines.size() };
    int res = 0;
    if (!prog.is_used("DiffEntries")) {
        res = do_char_diff_loop(prog.get<bool>("--lcs"));
    }
    else {
        auto fnames = prog.get<std::vector<std::string>>("DiffEntries");
        res = do_file_diff(fnames[0], fnames[1], prog.get<bool>("--lcs"));
    }

    return res;
}

int do_file_diff(std::string& filename_a, std::string& filename_b, bool lcs_included) {
    constexpr std::size_t CAP = 32;

    std::array<char, CAP> buf;

    std::ifstream s1{filename_a};
    if (!s1) {
        return 2;
    }
    std::string str_a;
    while (s1.read(buf.data(), buf.size())) {
        str_a.append(buf.data(), s1.gcount());
    }
    if (s1.gcount() > 0) {
        str_a.append(buf.data(), s1.gcount());
    }

    s1 = std::ifstream{ filename_b };

    if (!s1) {
        return 2;
    }
    std::string str_b;
    while (s1.read(buf.data(), buf.size())) {
        str_b.append(buf.data(), s1.gcount());
    }
    if (s1.gcount() > 0) {
        str_b.append(buf.data(), s1.gcount());
    }

    return do_in_mem_diff(str_a, str_b, lcs_included);
}

int do_char_diff_loop(bool lcs_included) {
    std::string s_a;
    std::string s_b;

    std::vector<std::pair<std::size_t, std::size_t>> pos_len{};
    while (std::getline(std::cin, s_a) && std::getline(std::cin, s_b)) {
        if (lcs_included) {
            auto result = try_ses_lcs(s_a, s_b);
            std::cout << result.first << "(E count is " << result.second << ")\n";
        }
        auto huhh = try_ses_diff(s_a, s_b);
        auto& bah = huhh.first;
#if 0
        str_s = std::istringstream{ s_a };
        while (str_s >> str_buf) {
            pos_len.push_back({ str_data.size(), str_buf.size() + 1 });
            str_data.append(str_buf);
            str_data.push_back('\n');
        }
#endif
    }
    return 0;
}
