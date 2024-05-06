
export module Statistics:Median;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <algorithm>;

export namespace Stats {
	template <NumericRange Range>
	auto median(Range&& values, bool isSorted = false) -> f64 {
		const auto len = values.size();
		if (len == 0) return 0.0;
		const auto halfLen = len / 2;
		if (isSorted) { // avoid copy and sort
			if (len % 2 == 0)
				return static_cast<f64>(values[halfLen] + values[halfLen - 1]) / 2.0;
			return static_cast<f64>(values[halfLen]);
		} // if not sorted, make a copy and sort the copy then grab values
		auto vals = values; // copy values
		std::ranges::sort(vals); // default is sort low to high w/ < operator
		if (len % 2 == 0)
			return static_cast<f64>(vals[halfLen] + vals[halfLen - 1]) / 2.0;
		return static_cast<f64>(vals[halfLen]);
	}
};
