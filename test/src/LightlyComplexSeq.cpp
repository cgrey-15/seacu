#include "seacudiff.h"
#include <bit>
#include <iostream>
#include <string>
int LightlyComplexSeq(int argc, char* argv[]) {


	auto t00 = try_ses_diff_s("abcabba", "cbabac");
	auto t01 = try_ses_diff_s("whatisdamenupofyk", "whatisthemean");
	auto t02 = try_ses_diff_s("abcdefghijklmnopqrs", "abdefghjkqrABs");
	auto t03 = try_ses_diff_s("ABCabcdefghijklm", "ABCdegno");
	return 0;
}
