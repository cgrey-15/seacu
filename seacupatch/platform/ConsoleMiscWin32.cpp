#include "ConsoleMisc.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
//#include <consoleapi.h>

namespace seacu {
	constexpr UINT UTF8_CODEPAGE = 65001U;
}

bool using_utf8() {
	auto in_cp = GetConsoleCP();
	auto out_cp = GetConsoleOutputCP();

	return in_cp == seacu::UTF8_CODEPAGE && out_cp == seacu::UTF8_CODEPAGE;
}
