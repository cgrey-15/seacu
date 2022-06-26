#include "seacudiff.h"

int MatchingAndSimpleSeq(int argc, char* argv[]) {
	auto t00 = try_ses_diff_s("hi", "h");
	auto t01 = try_ses_diff_s("a", "a");
	auto t02 = try_ses_diff_s("abcd", "abcd");
	auto t03 = try_ses_diff_s("abc", "ab");
	auto t04 = try_ses_diff_s("abcde", "ab");
	auto t05 = try_ses_diff_s("thisarecown", "t");
	auto t06 = try_ses_diff_s("thisarecown", "th");
	auto t07 = try_ses_diff_s("ThisAreCown", "ThisAreCownB");
	return 0;
}
