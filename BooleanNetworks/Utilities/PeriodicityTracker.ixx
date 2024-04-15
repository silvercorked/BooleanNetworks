
export module PeriodicityTracker;

import PrimitiveTypes;

import PackedBoolVector;

import <deque>;

// could likely template this for any accessible iterator
export class PeriodicityTracker {
	std::deque<PackedBoolVector> prevIterations;
	const u32 depth;
	
public:
	PeriodicityTracker(u32);
	~PeriodicityTracker();

	auto addSample(const PackedBoolVector&) -> bool;
	auto isPeriodic() const -> bool;
};