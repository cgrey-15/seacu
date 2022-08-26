#include "seacupatch.h"
#include "platform/ConsoleMisc.h"
//#include <argparse/argparse.hpp>
#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <fstream>
#include <bit>
#include "parsing.hpp"

int do_something(const std::string& filename_a, const std::string& filename_b);

int main(int argc, const char* argv[]) {

	//argparse::ArgumentParser progi{ "LibseacudiffRunner", "0.0.5" };
	int ret = 0;

	cxxopts::Options opts{ "libseacupatchrunner", "Test program to patch text files (currently rudimentarily)" };
	cxxopts::ParseResult parseRes;
	{
		using namespace std::string_literals;
#if 0
		progi.add_argument("ORIGFILE")
			.default_value(""s);
		progi.add_argument("PatchFILE")
			.default_value(""s);
#endif

		opts.add_options()("ORIGFILE", "Original file to process", cxxopts::value<std::string>()->default_value(""s))
			("PatchFILE", "A delta file corresponding to original file", cxxopts::value<std::string>()->default_value(""s));
		opts.parse_positional({ "ORIGFILE", "PatchFILE" });
	}

	try {
		//progi.parse_args(argc, argv);
		parseRes = opts.parse(argc, argv);
	}
	catch (const std::exception& e) {
#if 0
		std::cerr << e.what() << std::endl;
		std::cerr << progi;
		std::exit(1);
#endif
		std::cerr << e.what() << std::endl;
		std::exit(1);
	}

#if 0
	if (progi.is_used("ORIGFILE")) {
		auto arg = progi.get<std::string>("ORIGFILE");
		std::u8string str(std::bit_cast<char8_t*>(arg.data()), arg.size());

		if (progi.is_used("PatchFILE")) {
			auto arg2 = progi.get<std::string>("PatchFILE");
			std::cout << "and an optional arg was given: " << arg2 << "\n";
			auto resu = do_something(arg, arg2);
		}
	}
	else {
		if (! progi.is_used("PatchFILE")) {
			std::cout << "...unfortunately, no arguments were given.\n";
		}
		else {
			std::cerr << "...\n";
			std::exit(1);
		}
	}
#endif
	if (! parseRes["ORIGFILE"].has_default()) {
		auto& arg = parseRes["ORIGFILE"].as<std::string>();
		std::u8string str(std::bit_cast<char8_t*>(arg.data()), arg.size());

		if (!parseRes["PatchFILE"].has_default()) {
			auto& arg2 = parseRes["PatchFILE"].as<std::string>();
			std::cout << "and an optional arg was given: " << arg2 << "\n";
			ret = do_something(arg, arg2);
		}
	}
	else {
		if (parseRes["PatchFILE"].has_default()) {
			std::cout << "...unfortunately, no arguments were given.\n";
		}
		else {
			std::cerr << "...\n";
			std::exit(1);
		}
	}
	return ret;
}
int do_something(const std::string& filename_a, const std::string& filename_b) {
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
