module;

export module CellularAutomata;

import PrimitiveTypes;
import PackedBoolVector;

import <functional>;
import <cmath>;
import <span>;
import <random>;

export using RuleFunctionType = std::function<bool(std::vector<bool> const&)>;
export using MeanFieldApproximationFunctionType = std::function<f64(f64, u32)>;



export namespace CellularAutomataResources {
	namespace Utility {
		auto countHighs(std::vector<bool> const& values) -> u32;
		auto isTotalisingForSum(std::vector<bool> const& values, u32 totalisingSum) -> bool;
		auto isTotalisingForSums(std::vector<bool> const& values, std::vector<u32> const& totalisingSums) -> bool;
	};

	const RuleFunctionType ruleFunc22 = [](std::vector<bool> const& bools) -> bool {
		return Utility::isTotalisingForSum(bools, 1);
	};
	const RuleFunctionType ruleFunc126 = [](std::vector<bool> const& bools) -> bool {
		std::vector<u32> counts;
		counts.reserve(bools.size() - 1);
		for (u32 i = 1; i < bools.size(); i++) // not 0 and not all bools high
			counts.push_back(i);
		return Utility::isTotalisingForSums(bools, counts);
	};
	const MeanFieldApproximationFunctionType meanFieldApproximationRule22 = [](f64 prevDensityOfOnes, u32 parentCount) -> f64 {
		return (1 + (parentCount - 1)) * prevDensityOfOnes * pow(1 - prevDensityOfOnes, parentCount - 1); // pg 33, p(t+1) = (1+k)p(t)(1-p(t))^k, where k is 1 less than number of inputs
	};
	const MeanFieldApproximationFunctionType meanFieldApproximationRule126 = [](f64 prevDensityOfOnes, u32 parentCount) -> f64 {
		return 1 - pow(prevDensityOfOnes, parentCount) - pow(1 - prevDensityOfOnes, parentCount); // pg 28, p(t+1) = 1 - p(t)^(k+1) - (1 - p(t))^(k+1), where k is 1 less than number of inputs
	};
};

export enum CellularAutomataParentConfigEnum {
	RANDOMIZE_PARENTS,
	NEIGHBORHOOD_PARENTS
};
export enum CellularAutomataInitialConfigEnum {
	RANDOM,
	ONE_HIGH
};

export class CellularAutomata {
	PackedBoolVector data;
	PackedBoolVector data2;
	bool which;
	std::unordered_map<u32, std::vector<u32>> indexMap;
	const RuleFunctionType ruleFunc;
	const MeanFieldApproximationFunctionType meanFieldApproximationFunc; // expects parents are randomly assigned
	f64 estimate;
	f64 actual;
	const u32 k;
	std::mt19937_64 gen;

public:
	CellularAutomata(
		u32 size,
		u32 parentCount,
		RuleFunctionType ruleFunction,
		MeanFieldApproximationFunctionType meanFieldApproximationFunction,
		CellularAutomataParentConfigEnum parentSetup,
		CellularAutomataInitialConfigEnum initialDataSetup
	);
	~CellularAutomata() {};

	auto go() -> void;
	auto go(i32 times) -> void;
	auto gather() -> std::pair<f64, f64>;
	auto gather(i32 times) -> std::pair<std::vector<f64>, std::vector<f64>>;

	auto getEstimate() -> f64;
	auto getActual() -> f64;
	auto getError() -> f64;

	auto setEstimate(f64 nEstimate) -> void;

private:
	auto getCurrentDensity() -> f64;
	
	auto setupNonRandomParents() -> void;
	auto setupRandomParents() -> void;

	auto randomizeCells() -> void;
	auto oneHighCell() -> void;
};
