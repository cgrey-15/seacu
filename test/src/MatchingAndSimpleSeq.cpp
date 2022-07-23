#include "seacudiff.h"

int MatchingAndSimpleSeq(int argc, char* argv[]) {
#if 0
	using namespace std::string_literals;
	int* ptr;
	const std::string fun_how = "Hello there ladies and gentlemen. Thank you for coming to tonight's open mic. We hope you enjoy yourselves! That's all for tonight."s;
	const volatile int VAL1 = 0x12344321;
	const int VAL2 = VAL1;
	const int VAL3 = VAL1;
	int const volatile* a_ptr = &VAL1;

	unsigned char pipes[100];
	ptr = std::bit_cast<int*>(&pipes[1]);
	int i = 0;
	while (i < 70) {
		*ptr = 0;
		++ptr;
		++i;
	}

	std::cout << VAL3 << "\n";
	std::cout << *a_ptr << "\n";
	//std::cout << fun_how << "\n";
#endif

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
