#include <gsl/pointers>
#include <iosfwd>

namespace seacudiff {
	struct diff_options {
		diff_options(std::ostream& o) : file{ &o } {}
		gsl::not_null<std::ostream*> file;
	};
}
