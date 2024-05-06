module;

export module BooleanNetwork:CellularAutomataResources;

import PrimitiveTypes;

import <functional>;
import <vector>;

export namespace CellularAutomataResources {
	using RuleFunctionType = std::function<bool(std::vector<bool> const&)>;
	using MeanFieldApproximationFunctionType = std::function<f64(f64, u32)>;

	enum CellularAutomataParentConfigEnum {
		RANDOMIZE_PARENTS,
		NEIGHBORHOOD_PARENTS
	};
	enum CellularAutomataInitialConfigEnum {
		RANDOM,
		ONE_HIGH
	};

	namespace Utility {
		auto countHighs(const std::vector<bool>& values) -> u32 {
			u32 count = 0;
			for (const bool val : values)
				if (val) count++;
			return count;
		}
		auto isTotalisingFor(const std::vector<bool>& values, const u32 totalisingSum) -> bool {
			return countHighs(values) == totalisingSum;
		}
		auto isTotalisingForAnyOf(const std::vector<bool>& values, const std::vector<u32>& totalisingSums) -> bool {
			bool any = false;
			const u32 highCount = countHighs(values);
			for (const u32 totalisingSum : totalisingSums)
				any |= highCount == totalisingSum;
			return any;
		}
	};

	const RuleFunctionType ruleFunc22 = [](std::vector<bool> const& bools) -> bool {
		return Utility::isTotalisingFor(bools, 1);
	};
	const RuleFunctionType ruleFunc126 = [](std::vector<bool> const& bools) -> bool {
		std::vector<u32> counts;
		counts.reserve(bools.size() - 1);
		for (u32 i = 1; i < bools.size(); i++) // not 0 and not all bools high
			counts.push_back(i);
		return Utility::isTotalisingForAnyOf(bools, counts);
	};
	const MeanFieldApproximationFunctionType meanFieldApproximationRule22 = [](f64 prevDensityOfOnes, u32 parentCount) -> f64 {
		return static_cast<f64>(parentCount) * prevDensityOfOnes * pow(1.0 - prevDensityOfOnes, static_cast<f64>(parentCount - 1)); // pg 33, p(t+1) = (1+k)p(t)(1-p(t))^k, where k is 1 less than number of inputs
	};
	const MeanFieldApproximationFunctionType meanFieldApproximationRule126 = [](f64 prevDensityOfOnes, u32 parentCount) -> f64 {
		return 1 - pow(prevDensityOfOnes, parentCount) - pow(1 - prevDensityOfOnes, parentCount); // pg 28, p(t+1) = 1 - p(t)^(k+1) - (1 - p(t))^(k+1), where k is 1 less than number of inputs
	};
}
