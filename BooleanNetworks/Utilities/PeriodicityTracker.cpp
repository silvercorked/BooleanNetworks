
module PeriodicityTracker;

import <stdexcept>;

PeriodicityTracker::PeriodicityTracker(u32 depth) :
	depth(depth),
	prevSamples()
{
	if (depth < 2)
		throw std::invalid_argument("depth must be greater than 2 to find periodicity.");
}
PeriodicityTracker::~PeriodicityTracker() {};

auto PeriodicityTracker::checkForPeriodicityOfRecent() -> bool {
	const auto len = this->prevSamples.size();
	if (len < 2) return false; // can't have periodicity without at least 2 elements in tracker
	const auto pop1 = this->prevSamples.at(len - 1).popcount();
	for (i32 i = 0; i < len - 1; i++) { // last is most recent. check that against all others
		const auto pop2 = this->prevSamples.at(i).popcount();
		if (this->prevSamples.at(len - 1) == this->prevSamples.at(i))
			return true;
	}
	return false;
}
auto PeriodicityTracker::checkForPeriodicityOfAll() -> bool {
	const auto len = this->prevSamples.size();
	if (len < 2) return false; // can't have periodicity without at least 2 elements in tracker
	for (i32 i = 0; i < len; i++) {
		for (i32 j = i; j < len; j++) {
			if (this->prevSamples.at(i) == this->prevSamples.at(j))
				return true;
		}
	}
	return false;
}
auto PeriodicityTracker::addSample(const PackedBoolVector& nSample) -> void {
	if (this->prevSamples.size() - 1 == this->depth)
		this->prevSamples.pop_front(); // drop front
	this->prevSamples.push_back(nSample); // add to back
}
auto PeriodicityTracker::addSample(const PackedBoolVector&& nSample) -> void {
	if (this->prevSamples.size() - 1 == this->depth)
		this->prevSamples.pop_front(); // drop front
	this->prevSamples.push_back(std::move(nSample)); // add to back
}