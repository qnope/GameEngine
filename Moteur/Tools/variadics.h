#pragma once

namespace impl {
	struct expand {
		template<typename ...T>
		expand(T &&...) {}
	};
}

#define EXPAND_FUNCTION(PATTERN) ::impl::expand{((PATTERN), void(), 0)...}
