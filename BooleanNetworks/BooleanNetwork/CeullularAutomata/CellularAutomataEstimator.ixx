module;

export module BooleanNetwork:CellularAutomataEstimator;

import PrimitiveTypes;

import :CellularAutomataResources;

export class CellularAutomataEstimator {
	const CellularAutomataResources::MeanFieldApproximationFunctionType meanFieldApproximationFunc; // expects parents are randomly assigned
	f64 estimate;
	const u32 k;

public:
	CellularAutomataEstimator(
		u32 parentCount,
		CellularAutomataResources::MeanFieldApproximationFunctionType meanFieldApproximationFunction
	);
	~CellularAutomataEstimator() = default;

	auto go() -> void;
	auto go(i32 times) -> void;
	auto gather() -> f64;
	auto gather(i32 times) -> std::vector<f64>;

	auto getK() const->u32;
	auto getEstimate() const->f64;

	auto setEstimate(f64 nEstimate) -> void;
};

CellularAutomataEstimator::CellularAutomataEstimator(
	u32 parentCount,
	CellularAutomataResources::MeanFieldApproximationFunctionType meanFieldApproximationFunction
) :
	estimate(0.5),
	meanFieldApproximationFunc(
		meanFieldApproximationFunction
	),
	k(parentCount)
{}

auto CellularAutomataEstimator::go() -> void {
	this->estimate = this->meanFieldApproximationFunc(this->estimate, this->k);
}
auto CellularAutomataEstimator::go(i32 times) -> void {
	for (i32 i = 0; i < times; i++) {
		this->go();
	}
}
auto CellularAutomataEstimator::gather() -> f64 {
	this->go();
	return this->estimate;
}
auto CellularAutomataEstimator::gather(i32 times) -> std::vector<f64> {
	std::vector<f64> estimates;
	estimates.reserve(times);
	for (i32 i = 0; i < times; i++) {
		estimates.push_back(this->gather());
	}
	return estimates;
}

auto CellularAutomataEstimator::getK() const->u32 {
	return this->k;
}
auto CellularAutomataEstimator::getEstimate() const->f64 {
	return this->estimate;
}

auto CellularAutomataEstimator::setEstimate(f64 nEstimate) -> void {
	this->estimate = nEstimate;
}
