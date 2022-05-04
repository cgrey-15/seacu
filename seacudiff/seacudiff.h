#include <vector>
#include <string>

namespace seacu {
	struct some_edit_t {
			char a;
	};

	struct some_type_t {
			char b;
	};

	struct edit_region_t {
			char c;
	};

	edit_region_t find_lcsFOO(some_type_t a, some_type_t b, some_edit_t r);
}
namespace seacudiff {
	class SimpleSequence {
		std::vector<uint16_t> h;
	public:
		explicit SimpleSequence(std::string_view* b, std::string_view* e);
	};
}

