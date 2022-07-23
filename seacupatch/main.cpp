#include "seacupatch.h"
#include "platform/ConsoleMisc.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <fstream>
#include <bit>
#include "parsing.hpp"

int do_something(std::string& filename_a, std::string& filename_b);

int main(int argc, const char* argv[]) {

	argparse::ArgumentParser prog{ "LibseacudiffRunner", "0.0.5" };

	{
		using namespace std::string_literals;
		prog.add_argument("ORIGFILE")
			.default_value(""s);
		prog.add_argument("PatchFILE")
			.default_value(""s);
	}

	try {
		prog.parse_args(argc, argv);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << prog;
		std::exit(1);
	}

	if (prog.is_used("ORIGFILE")) {
		auto arg = prog.get<std::string>("ORIGFILE");
		std::u8string str(std::bit_cast<char8_t*>(arg.data()), arg.size());

		if (prog.is_used("PatchFILE")) {
			auto arg2 = prog.get<std::string>("PatchFILE");
			std::cout << "and an optional arg was given: " << arg2 << "\n";
			auto resu = do_something(arg, arg2);
		}
	}
	else {
		if (!prog.is_used("PatchFILE")) {
			std::cout << "...unfortunately, no arguments were given.\n";
		}
		else {
			std::cerr << "...\n";
			std::exit(1);
		}
	}
	return 0;
}
int do_something(std::string& filename_a, std::string& filename_b) {
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

	return seacupatch::patch(str_a, str_b);
}
