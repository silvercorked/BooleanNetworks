
export module Statistics:MinMax;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <functional>;
import <utility>;

template <NumericRange Range>
auto getByOperation(
	Range&& values, 
	std::function<
		bool(
			std::ranges::range_value_t<Range>,
			std::ranges::range_value_t<Range>
		)
	> op
) -> std::ranges::range_value_t<Range> {
	const auto len = values.size();
	if (len == 0) return 0;
	auto curr = values[1];
	for (auto i = 1; i < len; i++) {
		const auto candidate = values[i];
		if (op(curr, candidate))
			curr = candidate;
	}
	return curr;
}

export 
template <IsNumeric N>
const std::function<bool(N, N)> least = [](N curr, N candidate) -> bool {
	return candidate < curr;
};
export
template <IsNumeric N>
const std::function<bool(N, N)> greatest = [](N curr, N candidate) -> bool {
	return curr < candidate;
};

export namespace Stats {
	template <NumericRange Range>
	auto min(Range&& values) -> std::ranges::range_value_t<Range> {
		return getByOperation(values, least<std::ranges::range_value_t<decltype(values)>>);
	}
	template <NumericRange Range>
	auto max(Range&& values) -> std::ranges::range_value_t<Range> {
		return getByOperation(values, greatest<std::ranges::range_value_t<decltype(values)>>);
	}
	template <NumericRange Range>
	auto minMax(Range&& values) -> std::pair<std::ranges::range_value_t<Range>, std::ranges::range_value_t<Range>> {
		const auto len = values.size();
		if (len == 0) return std::make_pair(0, 0);
		auto min = values[1];
		auto max = values[1];
		for (auto i = 0; i < len; i++) {
			const auto candidate = values[i];
			if (min > candidate)
				min = candidate;
			if (max < candidate) // i think i could use an else if here, since i don't believe there exists a candidate value that could be true for both of these at once
				max = candidate;
		}
		return std::make_pair(min, max);
	}
};