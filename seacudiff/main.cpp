#include "seacudiff.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <array>
#include <string_view>
#include <tuple>
#include <iostream>
#include <sstream>
#include <fstream>

auto try_ses_lcs_l(std::string_view a, std::string_view b, bool do_backwards = false)->std::pair<std::vector<uint32_t>, size_t>;
int do_char_diff(std::string_view v_a, std::string_view v_b, bool lcs_included, bool disable_diff);
int do_char_diff_loop(bool lcs_included, bool disable_diff);
int do_file_diff(std::string& filename_a, std::string& filename_b);


int main(int argc, char* argv[]) {
    //using namespace seacu;

    bool use_backwards = false;

    argparse::ArgumentParser prog{ "test_prog", "0.0.5" };

    {
        using namespace std::string_literals;
        prog.add_argument("DiffEntries").default_value(std::vector<std::string>{""s,""s}).nargs(2);
        prog.add_argument("--lcs").implicit_value(true).default_value(false);
        prog.add_argument("--no-diff").implicit_value(true).default_value(false);
        prog.add_argument("-i", "--letter-symbol-inputs").default_value(std::vector<std::string>{""s, ""s}).nargs(2);
    }
    try {
        prog.parse_args(argc, argv);
    }
    catch (std::runtime_error& e) {
        std::cerr << "argparse: " << e.what() << std::endl;
        std::exit(1);
    }

    int res = 0;

    if (prog.is_used("DiffEntries")) {
        if (prog.is_used("-i") || prog.is_used("--lcs")) {
            if (prog.is_used("-i")) {
                std::cerr << " test_prog: -i/--letter-sym... incompatible with given filenames; use either one or the other" << std::endl;
            }
            if (prog.is_used("--lcs")) {
                std::cerr << "test_prog: --lcs cannot be used with file diffs of lines." << std::endl;
            }
            res = 1;
        }
        else {
            auto fnames = prog.get<std::vector<std::string>>("DiffEntries");
            res = do_file_diff(fnames[0], fnames[1]);
        }
    }
    else if (prog.is_used("-i")) {
        auto snames = prog.get<std::vector<std::string>>("-i");
        res = do_char_diff(snames[0], snames[1], prog.get<bool>("--lcs"), prog.get<bool>("--no-diff"));
    }
    else {
        res = do_char_diff_loop(prog.get<bool>("--lcs"), prog.get<bool>("--no-diff"));
    }

    return res;
}

int do_file_diff(std::string& filename_a, std::string& filename_b) {
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

    return do_in_mem_diff(str_a, str_b, std::cout);
}

int do_char_diff(std::string_view v_a, std::string_view v_b, bool lcs_included, bool disable_diff) {
    std::string aux_buf;

    if (v_a.empty() || v_b.empty()) {
        std::getline(std::cin, aux_buf);
        v_a.empty() ? v_a = aux_buf : v_b = aux_buf;
    }
    if (lcs_included || disable_diff) {
        auto result0 = try_ses_lcs_s(v_a, v_b);
        auto result = std::string_view{ result0.first.begin(), result0.first.end() };
        std::cout << result << "(E count is " << result0.second << ")\n";
    }
    if (!disable_diff) {
        auto huhh = try_ses_diff_s(v_a, v_b);
        auto& bah = huhh.first;
    }
    return 0;
}

int do_char_diff_loop(bool lcs_included, bool disable_diff) {
    std::string s_a;
    std::string s_b;

    std::vector<std::pair<std::size_t, std::size_t>> pos_len{};
    while (std::getline(std::cin, s_a) && std::getline(std::cin, s_b)) {
        if (lcs_included || disable_diff) {
            auto result0 = try_ses_lcs_s(s_a, s_b);
            auto result = std::string_view{ result0.first.begin(), result0.first.end()};
            std::cout << result << "(E count is " << result0.second << ")\n";
        }
        if (!disable_diff) {
            auto huhh = try_ses_diff_s(s_a, s_b);
            auto& bah = huhh.first;
        }

    }
    return 0;
}
