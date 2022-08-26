#include "seacudiff.h"
//#include <argparse/argparse.hpp>
#define CXXOPTS_NO_REGEX
#include <cxxopts.hpp>
#undef CXXOPTS_NO_REGEX
#include <ios>
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
int do_file_diff(const std::string& filename_a, const std::string& filename_b);

#if 0
template<typename T, size_t I>
std::istream& operator>>(std::istream& s, std::array<T, I>& arr) {
    for (T& el : arr) {
        std::getline(s, el, s.widen(','));
    }
    return s;
}
#endif

int main(int argc, char* argv[]) {
    //using namespace seacu;

    bool use_backwards = false;

    //argparse::ArgumentParser prog{ "test_prog", "0.0.5" };
    cxxopts::Options prog{ "test_prog", "Test program to diff files" };
    cxxopts::ParseResult opts{};
    //std::string tempStr;
    //std::getline(std::cin, tempStr);

    {
        using namespace std::string_literals;
        //prog.add_argument("DiffEntries").default_value(std::vector<std::string>{""s,""s}).nargs(2);
        prog.add_options()("DiffEntryA", "First file", cxxopts::value<std::string>()->default_value(""))
            ("DiffEntryB", "Second file", cxxopts::value<std::string>()->default_value(""))
            //prog.add_argument("--lcs").implicit_value(true).default_value(false);
            ("lcs", "Find longest common sequence instead", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
            //prog.add_argument("--no-diff").implicit_value(true).default_value(false);
            ("no-diff", "Don't diff compare", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
            //prog.add_argument("-i", "--letter-symbol-inputs").default_value(std::vector<std::string>{""s, ""s}).nargs(2);
            ("i,letter-symbol-inputs", "Compare two words instead", cxxopts::value<std::vector<std::string>>()->default_value("\"\" \"\""));
        prog.parse_positional({ "DiffEntryA", "DiffEntryB"});
    }
    try {
        //prog.parse_args(argc, argv);
        opts = prog.parse(argc, argv);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "cxxopts: " << e.what() << std::endl;
        std::exit(1);
    }
#if 1
    catch (const std::exception& e) {
        std::cerr << "cxxopts: " << e.what() << std::endl;
        std::exit(1);
    }
#endif

    int res = 0;

#if 1
    //if (prog.is_used("DiffEntries")) {
    if(! (opts["DiffEntryA"].has_default() && opts["DiffEntryB"].has_default()) ) {
        //if (prog.is_used("-i") || prog.is_used("--lcs")) {
        if (opts["DiffEntryB"].has_default()) {
            std::cerr << "test_prog: only one file was given (two arguments required)" << std::endl;
            res = 1;
        }
        else if(! (opts["i"].has_default() && opts["lcs"].has_default()) ) {
            //if (prog.is_used("-i")) {
            if(! opts["i"].has_default()) {
                std::cerr << " test_prog: -i/--letter-sym... incompatible with given filenames; use either one or the other" << std::endl;
            }
            //if (prog.is_used("--lcs")) {
            if (! opts["lcs"].has_default()) {
                std::cerr << "test_prog: --lcs cannot be used with file diffs of lines." << std::endl;
            }
            res = 1;
        }
        else {
            //auto fnames = prog.get<std::vector<std::string>>("DiffEntries");
            auto& fnameA = opts["DiffEntryA"].as<std::string>();
            auto& fnameB = opts["DiffEntryB"].as<std::string>();
            res = do_file_diff(fnameA, fnameB);
        }
    }
    //else if (prog.is_used("-i")) {
    else if (! opts["i"].has_default()) {
        if (auto& snames = opts["i"].as<std::vector<std::string>>();
            snames.size() != 2 || snames[0].empty())
        {
            std::cerr << "test_prog: -i expects 2 arguments but got: " << opts["i"].count() << " argument(s)" << std::endl;
            res = 1;
        }
        else {
            //auto snames = prog.get<std::vector<std::string>>("-i");
            
            //res = do_char_diff(snames[0], snames[1], prog.get<bool>("--lcs"), prog.get<bool>("--no-diff"));
            res = do_char_diff(snames[0], snames[1], opts["lcs"].as<bool>(), opts["no-diff"].as<bool>());
        }
    }
    else {
        //res = do_char_diff_loop(prog.get<bool>("--lcs"), prog.get<bool>("--no-diff"));
        res = do_char_diff_loop(opts["lcs"].as<bool>(), opts["no-diff"].as<bool>());
    }
#endif

    return res;
}

int do_file_diff(const std::string& filename_a, const std::string& filename_b) {
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
