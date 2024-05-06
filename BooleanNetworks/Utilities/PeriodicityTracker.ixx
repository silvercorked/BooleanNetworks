
export module PeriodicityTracker;

import PrimitiveTypes;

import PackedBoolVector;

import <deque>;

export class PeriodicityTracker {
	const u32 depth;
	std::deque<PackedBoolVector> prevSamples;

public:
	PeriodicityTracker(u32);
	~PeriodicityTracker();

	auto checkForPeriodicityOfRecent() -> bool;
	auto checkForPeriodicityOfAll() -> bool;
	auto addSample(const PackedBoolVector&) -> void;
	auto addSample(const PackedBoolVector&&) -> void;
	auto checkForPeriodicityOfRecentWithThreads() -> bool;
};