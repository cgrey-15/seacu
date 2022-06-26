#ifndef DIFF_EMIT_H
#define DIFF_EMIT_H
#pragma once
#include "seacudiff.h"
#include "SeacudiffTypes.h"
#include "diff_opt.h"

namespace seacudiff {
	using emit_callback_f = void (*)(void*, char const*, diff_options&);
	struct diff_emit_t {
		emit_callback_f line_fn;
		diff_emit_t(const diff_options& o) noexcept;
		int foo();
		int operator()(std::vector<edit_t<std::uint32_t>>& vec, const DiffEntrySeacufiles& seacudf1, const DiffEntrySeacufiles& seacudf2);
	private:
		diff_options o_;
	};
}

#endif //DIFF_EMIT_H


