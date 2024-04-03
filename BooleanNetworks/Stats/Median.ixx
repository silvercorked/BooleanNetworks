
export module Statistics:Median;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <algorithm>;

export namespace Stats {
	template <NumericRange Range>
	auto median(Range&& values, bool isSorted = false) -> f64 {
		size_t len = values.size();
		if (len == 0) return 0.0;
		if (!isSorted) std::ranges::sort(values); // default is sort low to high w/ < operator
		size_t halfLen = len / 2;
		if (len % 2 == 0)
			return static_cast<f64>(values[halfLen] + values[halfLen - 1]) / 2.0;
		return static_cast<f64>(values[halfLen]);
	}
};
