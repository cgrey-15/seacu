#ifdef SEACULOGGER_HEADER_ONLY
#include "seaculogger.hpp"
#else
#include "seaculogger.h"
#endif
#include <array>
#include <iterator>
#include <type_traits>
#include <ranges>
#include <iostream>
#include <limits>

template <std::random_access_iterator It>
It coolFunc(const It& it) {
	return It{};
}

int LoggerIO(int argc, char* argv[]) {
	using slogger = seacu::logger<char>;

	slogger l;

	bool res = true;

	constexpr auto funtimes = std::numeric_limits<std::int64_t>::min();
	l << "This is\ncool and ";
	l << "This is actually great\nAwesome! And now";
	l << " " << funtimes << " is the magic number\n";
	l << "and\nnow you know why!";

	constexpr std::array<char const*,5> lTest = { "This is\n", 
		"cool and This is actually great\n",
		"Awesome! And now -9,223,372,036,854,775,808 is the magic number\n",
	    "and\n",
	    "now you know why!"};

	for (auto line = l.begin(); line != l.end(); ++line) {
		res = res && (*line == lTest[line-l.begin()]);
	}

	//coolFunc(l);

#if 0
	for (auto el : std::ranges::subrange(l.end() - 2, l.end())) {
#else
	for (auto el : l) {
#endif
		std::cout << el;
	}
	std::cout << "End of Test!\n";

	return res ? 0 : 1;
}
