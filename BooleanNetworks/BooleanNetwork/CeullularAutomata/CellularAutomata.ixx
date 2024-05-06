module;

export module BooleanNetwork:CellularAutomata;

import PrimitiveTypes;

import :CellularAutomataResources;

import ThreadPool;
import PackedBoolVector;

import <functional>;
import <cmath>;
import <span>;
import <random>;
import <set>;
import <stdexcept>;
import <thread>;
import <chrono>;
import <execution>;
import <algorithm>;

export class CellularAutomata {
	PackedBoolVector data;
	PackedBoolVector data2;
	bool which;
	std::unordered_map<u32, std::vector<u32>> indexMap;
	const CellularAutomataResources::RuleFunctionType ruleFunc;
	f64 actual;
	const u32 k;
	std::mt19937_64 gen;

public:
	CellularAutomata(
		u32 size,
		u32 parentCount,
		CellularAutomataResources::RuleFunctionType ruleFunction,
		CellularAutomataResources::CellularAutomataParentConfigEnum parentSetup,
		CellularAutomataResources::CellularAutomataInitialConfigEnum initialDataSetup
	);
	~CellularAutomata() = default;

	auto go() -> void;
	auto go(const i32 times) -> void;
	auto gather() -> f64;
	auto gather(const i32 times) -> std::vector<f64>;

	auto goWithThreads(const i32 numThreads) -> void;
	auto gatherWithThreads(const i32 numThreads) -> f64;

	auto getK() const -> u32;
	auto getActual() const -> f64;

	auto getNodeValues() const -> PackedBoolVector;

private:
	auto getCurrentDensity() -> f64;
	
	auto setupNonRandomParents() -> void;
	auto setupRandomParents() -> void;

	auto randomizeCells() -> void;
	auto oneHighCell() -> void;
};

CellularAutomata::CellularAutomata(
	u32 size,
	u32 parentCount,
	CellularAutomataResources::RuleFunctionType ruleFunction,
	CellularAutomataResources::CellularAutomataParentConfigEnum parentSetup,
	CellularAutomataResources::CellularAutomataInitialConfigEnum initialDataSetup
) :
	data(size),
	data2(size),
	which(false),
	actual(0.5),
	ruleFunc(
		ruleFunction
	),
	k(parentCount),
	indexMap(),
	gen(std::chrono::system_clock::now().time_since_epoch().count())
{
	if (size == 0)
		std::runtime_error("Cannot be of size 0.");

	if (parentSetup == CellularAutomataResources::CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS)
		this->setupRandomParents();
	else if (parentSetup == CellularAutomataResources::CellularAutomataParentConfigEnum::NEIGHBORHOOD_PARENTS)
		this->setupNonRandomParents();
	else
		std::runtime_error("Invalid CellularAutomataParentConfigEnum value.");

	if (initialDataSetup == CellularAutomataResources::CellularAutomataInitialConfigEnum::RANDOM)
		this->randomizeCells();
	else if (initialDataSetup == CellularAutomataResources::CellularAutomataInitialConfigEnum::ONE_HIGH)
		this->oneHighCell();
	else
		std::runtime_error("Invalid CellularAutomataInitialConfigEnum value.");
}

auto CellularAutomata::go() -> void {
	auto& writeTo = (this->which ? this->data : this->data2);
	auto& readFrom = (this->which ? this->data2 : this->data);
	const i32 size = this->data.size();
	for (i32 i = 0; i < size; i++) {
		std::vector<bool> args;
		args.reserve(this->k);
		for (auto j : this->indexMap[i])
			args.push_back(readFrom.at(j));
		writeTo.setTo(i, this->ruleFunc(args));
	}
	// now update actual
	this->actual = this->getCurrentDensity();
	this->which = !this->which;
}
auto CellularAutomata::go(const i32 times) -> void {
	for (i32 i = 0; i < times; i++) {
		this->go();
	}
}
auto CellularAutomata::gather() -> f64 {
	this->go();
	return this->getActual();
}
auto CellularAutomata::gather(const i32 times) -> std::vector<f64> {
	std::vector<f64> actuals;
	actuals.reserve(times);
	for (i32 i = 0; i < times; i++) {
		actuals.push_back(this->gather());
	}
	return actuals;
}

auto CellularAutomata::goWithThreads(const i32 numThreads) -> void {
	if (numThreads < 1)
		throw std::runtime_error("must use at least 1 thread");
	auto& writeTo = (this->which ? this->data : this->data2);
	const auto& readFrom = (this->which ? this->data2 : this->data);
	const i32 size = this->data.size();
	const i32 numPerThread = size / numThreads;
	const i32 currThreadEnd = numPerThread;
	{
		std::unique_ptr<ThreadPool> tp;
		if (numThreads > 1) {
			tp = std::make_unique<ThreadPool>(numThreads - 1);
			for (i32 i = 1; i < numThreads; i++) {
				tp->queueTask([this, size, numThreads, numPerThread, i, &writeTo, &readFrom]() -> void {
					i32 end = i == numThreads - 1 ? size : (i + 1) * numPerThread;
					for (i32 j = i * numPerThread; j < end; j++) {
						std::vector<bool> args;
						args.reserve(this->k);
						for (const auto l : this->indexMap[j])
							args.push_back(readFrom.at(l));
						writeTo.setTo(j, this->ruleFunc(args));
					}
					});
			}
		}
		for (i32 i = 0; i < currThreadEnd; i++) { // do a portion while threads go
			std::vector<bool> args;
			args.reserve(this->k);
			for (auto j : this->indexMap[i])
				args.push_back(readFrom.at(j));
			writeTo.setTo(i, this->ruleFunc(args));
		}
		if (numThreads > 1)
			while (tp->busy()) { std::this_thread::yield(); } // wait for queue to empty (should already be empty)
	} // wait for threads to end work
	// now update actual
	this->which = !this->which;
	this->actual = this->getCurrentDensity();
}
auto CellularAutomata::gatherWithThreads(const i32 numThreads) -> f64 {
	this->goWithThreads(numThreads);
	return this->getActual();
}

auto CellularAutomata::getK() const -> u32 {
	return this->k;
}
auto CellularAutomata::getActual() const -> f64 {
	return this->actual;
}

auto CellularAutomata::getNodeValues() const -> PackedBoolVector {
	//auto& writeTo = (this->which ? this->data : this->data2);
	return this->which ? this->data2 : this->data; // inverse of go() function becasue go swaps this->which at the end
} // pass a copy to be messed around with but not allow messing with internals

auto CellularAutomata::setupNonRandomParents() -> void {
	const i32 halfK = ((this->k - 1) >> 1); // k = 3, means 2 other parents and 1 self refernce
	const i32 size = this->data.size();
	for (i32 i = 0; i < size; i++) {
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
	const i32 size = this->data.size();
	const u32 parentsToFind = this->k - 1;
	std::uniform_int_distribution<u32> dist(0, size - 1);
	for (i32 i = 0; i < size; i++) {
		std::set<u32> parentIndexes{};
		parentIndexes.insert(i);
		while (parentIndexes.size() <= parentsToFind)
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
