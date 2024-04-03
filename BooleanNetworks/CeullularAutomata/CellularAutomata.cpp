
module CellularAutomata;

import <set>;
import <stdexcept>;

namespace CellularAutomataResources {
	namespace Utility {
		auto countHighs(std::vector<bool> const& values) -> u32 {
			u32 count = 0;
			for (const bool val : values)
				if (val) count++;
			return count;
		}
		auto isTotalisingForSum(std::vector<bool> const& values, u32 totalisingSum) -> bool {
			return countHighs(values) == totalisingSum;
		}
		auto isTotalisingForSums(std::vector<bool> const& values, std::vector<u32> const& totalisingSums) -> bool {
			bool any = false;
			u32 highCount = countHighs(values);
			for (const u32 totalisingSum : totalisingSums)
				any |= highCount == totalisingSum;
			return any;
		}
	};
};

CellularAutomata::CellularAutomata(
	u32 size,
	u32 parentCount,
	RuleFunctionType ruleFunction,
	MeanFieldApproximationFunctionType meanFieldApproximationFunction,
	CellularAutomataParentConfigEnum parentSetup,
	CellularAutomataInitialConfigEnum initialDataSetup
) :
	data(size),
	data2(size),
	which(false),
	estimate(0.5),
	actual(0.5),
	ruleFunc(
		ruleFunction
	),
	meanFieldApproximationFunc(
		meanFieldApproximationFunction
	),
	k(parentCount),
	indexMap()
{
	if (size == 0)
		std::runtime_error("Cannot be of size 0.");
	this->gen.discard(10000);

	if (parentSetup == CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS)
		this->setupRandomParents();
	else if (parentSetup == CellularAutomataParentConfigEnum::NEIGHBORHOOD_PARENTS)
		this->setupNonRandomParents();
	else
		std::runtime_error("Invalid CellularAutomataParentConfigEnum value.");

	if (initialDataSetup == CellularAutomataInitialConfigEnum::RANDOM)
		this->randomizeCells();
	else if (initialDataSetup == CellularAutomataInitialConfigEnum::ONE_HIGH)
		this->oneHighCell();
	else
		std::runtime_error("Invalid CellularAutomataInitialConfigEnum value.");
}

auto CellularAutomata::go() -> void {
	auto& writeTo = (this->which ? this->data : this->data2);
	auto& readFrom = (this->which ? this->data2 : this->data);
	u32 i = 0;
	i32 halfK = ((this->k - 1) >> 1); // k = 3, means 2 other parents and 1 self refernce
	i32 size = this->data.size();
	for (; i < size; i++) {
		std::vector<bool> args;
		args.reserve(this->k);
		for (auto j : this->indexMap[i])
			args.push_back(readFrom.at(j));
		writeTo.setTo(i, this->ruleFunc(args));
	}
	// now update estimate and actual
	this->estimate = this->meanFieldApproximationFunc(this->estimate, this->k);
	this->actual = this->getCurrentDensity();
	this->which = !this->which;
}
auto CellularAutomata::go(i32 times) -> void {
	for (i32 i = 0; i < times; i++) {
		this->go();
	}
}
auto CellularAutomata::gather() -> std::pair<f64, f64> {
	this->go();
	return std::make_pair<f64, f64>(this->getEstimate(), this->getActual());
}
auto CellularAutomata::gather(i32 times) -> std::pair<std::vector<f64>, std::vector<f64>> {
	std::vector<f64> estimates;
	std::vector<f64> actuals;
	estimates.reserve(times);
	actuals.reserve(times);
	for (i32 i = 0; i < times; i++) {
		auto res = this->gather();
		estimates.push_back(res.first);
		actuals.push_back(res.second);
	}
	return std::make_pair(estimates, actuals);
}

auto CellularAutomata::getEstimate() -> f64 {
	return this->estimate;
}
auto CellularAutomata::getActual() -> f64 {
	return this->actual;
}
auto CellularAutomata::getError() -> f64 {
	return abs(this->actual - this->estimate);
}

auto CellularAutomata::setEstimate(f64 nEstimate) -> void {
	this->estimate = nEstimate;
}

auto CellularAutomata::setupNonRandomParents() -> void {
	u32 i = 0;
	const i32 halfK = ((this->k - 1) >> 1); // k = 3, means 2 other parents and 1 self refernce
	const i32 size = this->data.size();
	for (; i < size; i++) {
		std::vector<u32> args;
		args.reserve(k);
		for (i32 j = -halfK; j <= halfK; j++) {
			i32 index = i + j;
			if (index < 0) index = size + index;
			if (index >= size) index = j;
			args.push_back(index);
		}
		this->indexMap[i] = args;
	}
}
auto CellularAutomata::setupRandomParents() -> void {
	u32 i = 0;
	const i32 size = this->data.size();
	const u32 parentsToFind = this->k - 1;
	std::uniform_int_distribution<u32> dist(0, this->k - 1);
	for (; i < size; i++) {
		std::set<u32> parentIndexes{};
		parentIndexes.insert(i);
		while (parentIndexes.size() <= size)
			parentIndexes.insert(dist(this->gen));
		this->indexMap[i] = std::vector<u32>(parentIndexes.begin(), parentIndexes.end());
	}
}

auto CellularAutomata::getCurrentDensity() -> f64 {
	return static_cast<f64>(
		this->which ? this->data2.popcount() : this->data.popcount()
	) / static_cast<f64>(this->data.size());
}

auto CellularAutomata::randomizeCells() -> void {
	std::uniform_int_distribution dist(0, 1);
	for (i32 i = 0; i < static_cast<i32>(this->data.size()); i++) {
		this->data.setTo(i, static_cast<bool>(dist(this->gen)));
		this->data2.setTo(i, static_cast<bool>(dist(this->gen)));
	}
}
auto CellularAutomata::oneHighCell() -> void {
	this->data.clear(); // doesn't matter which cell is true, just that there's 1.
	this->data2.clear();
	this->data.setTo(0, true); // guarenteed size is at least 1
	this->data2.setTo(0, true);
}
