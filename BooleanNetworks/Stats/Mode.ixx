
export module Statistics:Mode;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <algorithm>;
import <unordered_map>;
import <vector>;

export namespace Stats {
	template <NumericRange Range>
	auto frequency(Range&& values) -> std::unordered_map<std::ranges::range_value_t<decltype(values)>, u32> {
		std::unordered_map<std::ranges::range_value_t<decltype(values)>, u32> freq{};
		const auto len = values.size();
		for (auto i = 0; i < len; i++) {
			if (freq.find(values[i]) != freq.end())
				freq[values[i]]++;
			else
				freq[values[i]] = 1;
		}
		return freq;
	}
	template <NumericRange Range>
	auto mode(Range&& values) -> std::vector<std::ranges::range_value_t<decltype(values)>> {
		std::unordered_map<std::ranges::range_value_t<decltype(values)>, u32> freq{};
		u32 largestCount = 1; // can start at 1 safetly, if empty, still gives empty array in the end
		const auto len = values.size();
		for (auto i = 0; i < len; i++) {
			if (freq.find(values[i]) != freq.end()) {
				freq[values[i]]++;
				if (freq[values[i]] > largestCount) largestCount++; // should be equivalent (since only even increment prev values) and avoids assignment and [] lookups
			}
			else
				freq[values[i]] = 1; // don't need to check cause count already starts at 1
		}
		std::vector<std::ranges::range_value_t<decltype(values)>> keyWithLargestCount{};
		for (const auto [value, count] : freq)
			if (count == largestCount) keyWithLargestCount.push_back(value);
		return keyWithLargestCount;
	}
};
