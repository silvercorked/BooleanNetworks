
export module Statistics:Midrange;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <algorithm>;

export namespace Stats {
	template <NumericRange Range>
	auto midrange(Range&& values, bool isSorted = false) -> f64 {
		size_t len = values.size();
		if (len == 0) return 0.0;
		if (!isSorted) std::ranges::sort(values); // default is sort low to high w/ < operator
		return static_cast<f64>(values[0] + values[len - 1]) / 2.0;
	}
};
