
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
		if (len == 0)
			throw std::runtime_error("cannot create ordinal ranks without elements. empty container.");
		std::vector<std::size_t> indexBlacklist;
		std::vector<std::size_t> out(len, 0);
		indexBlacklist.reserve(len);
		std::size_t currOrdinalVal = 1;
		while (indexBlacklist.size() != len) {
			std::size_t minIndex = 0;
			auto minVal = std::numeric_limits<std::ranges::range_value_t<Range>>::max();
			for (auto i = 0; i < len; i++) {
				bool found = false;
				for (const auto& index : indexBlacklist) {
					if (i == index) {
						found = true;
						break;
					}
				}
				if (found) continue;
				if (values[i] < minVal) {
					minVal = values[i];
					minIndex = i;
				}
			}
			out[minIndex] = currOrdinalVal;
			currOrdinalVal++;
			indexBlacklist.push_back(minIndex);
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