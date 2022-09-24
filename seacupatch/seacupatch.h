#include <string_view>

namespace seacupatch {
	struct cmd_opts{
		bool forceSort;
	};
	int patch(std::string_view buf_in, std::string_view buf_changes, cmd_opts progOpt);
}
