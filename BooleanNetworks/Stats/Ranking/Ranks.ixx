
export module Statistics:Ranks;

import PrimitiveTypes;

import :InternalUtilities;

import <array>;
import <stdexcept>;
import <algorithm>;

template <NumericRange Range>
auto findDupesIndexes(Range&& values, std::ranges::range_value_t<Range> val) -> std::vector<size_t> {
	auto dupeIndexes = std::vector<size_t>();
	const auto len = values.size();
	for (auto i = 0; i < len; i++) {
		if (values[i] == val)
			dupeIndexes.push_back(i);
	}
	return dupeIndexes;
}

export namespace Stats {
	template <NumericRange Range>
	auto getOrdinalRanks(
		Range&& values
	) -> std::vector<std::size_t> {
		const auto len = values.size();
		std::vector<std::ranges::range_value_t<Range>> vals(values); // get copy to modify
		std::vector<std::size_t> out;
		std::vector<std::size_t> maxIndexes;
		std::size_t currOrdinal = 1;
		// find max elements (ie, last elements)
		out.resize(len);
		auto max = std::numeric_limits<std::ranges::range_value_t<Range>>::min();
		for (const auto elem : vals) // figure out the max value
			if (elem > max)
				max = elem;
		for (auto i = 0; i < len; i++) // get all indexes that have the max value
			if (vals[i] == max)
				maxIndexes.push_back(i);
		// max is now a valid replacement for already found min values (can set each found min value with the max value to call min again)
		const auto maxIndexLen = maxIndexes.size();
		for (auto i = 0; i < len - maxIndexLen; i++) { // skip end, which is maxes, because we already have their numbers
			const auto forIter = std::min_element(vals.begin(), vals.end());
			const auto index = std::distance(vals.begin(), forIter);
			(*forIter) = max; // replace current min with max, allowing for the next min search to find a new index
			out[index] = currOrdinal; // order will fill out array 
			currOrdinal++; // start filling out
		}
		for (auto i = 0; i < maxIndexLen; i++) {
			out[maxIndexes[i]] = currOrdinal; // handle maxes, which were skipped by above loop
			currOrdinal++;
		}
		return out;
	}
	template <NumericRange Range>
	auto getFractionalRanks(
		Range&& values
	) -> const std::vector<f64> {
		auto ordinalVals = Stats::getOrdinalRanks(values);
		const auto len = ordinalVals.size();
		auto ret = std::vector<f64>(len, 0);
		auto indexBlacklist = std::vector<std::size_t>();
		indexBlacklist.reserve(len);
		size_t nextIndex = 0;
		while (indexBlacklist.size() != len) {
			auto dupeIndexes = findDupesIndexes(values, values[nextIndex]);
			double fractionalVal = 0;
			for (const auto& index : dupeIndexes) {
				fractionalVal += ordinalVals[index];
				indexBlacklist.push_back(index);
			}
			std::sort(indexBlacklist.begin(), indexBlacklist.end());
			fractionalVal /= dupeIndexes.size();
			for (auto i = 0; i < dupeIndexes.size(); i++) {
				ret[dupeIndexes[i]] = fractionalVal;
			}
			for (auto i = 0; i < ordinalVals.size(); i++) {
				if (i >= indexBlacklist.size() || i != indexBlacklist[i]) {
					nextIndex = i; // order is maintained, so as soon as this occurs, that's the next valid index
					break;
				}
			}
		}
		return ret;
	}
}