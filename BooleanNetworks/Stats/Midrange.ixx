
export module Statistics:Midrange;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <algorithm>;

export namespace Stats {
	template <NumericRange Range>
	auto midrange(Range&& values, bool isSorted = false) -> f64 {
		const auto len = values.size();
		if (len == 0) return 0.0;
		if (isSorted) // if sorted, avoid copy and sort
			return static_cast<f64>(values[0] + values[len - 1]) / 2.0;
		auto vals = values; // otherwise, make copy and sort it
		std::ranges::sort(vals); // default is sort low to high w/ < operator
		return static_cast<f64>(vals[0] + vals[len - 1]) / 2.0;
	}
};
