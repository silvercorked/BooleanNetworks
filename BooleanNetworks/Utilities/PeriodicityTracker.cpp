
module PeriodicityTracker;

import ThreadPool;

import <stdexcept>;
import <memory>;

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
	for (i32 i = 0; i < len - 1; i++) { // last is most recent. check that against all others
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

auto PeriodicityTracker::checkForPeriodicityOfRecentWithThreads(const u32 numThreads) -> bool {
	if (numThreads < 1)
		throw std::runtime_error("must use at least 1 thread");
	const auto len = this->prevSamples.size();
	if (len < 2) return false; // can't have periodicity without at least 2 elements in tracker
	bool result = false;
	const u32 mostRecent = len - 1;
	{
		std::unique_ptr<ThreadPool> tp;
		tp = std::make_unique<ThreadPool>(numThreads);
		for (i32 i = 0; i < mostRecent; i++) { // last is most recent. check that against all others
			tp->queueTask([this, i, mostRecent, &result]() -> void {
				if (this->prevSamples.at(mostRecent) == this->prevSamples.at(i))
					result = true;
			});
		}
		while (tp->busy() && !result) { std::this_thread::yield(); } // wait for queue to empty (should already be empty)
	} // if result is true, break loop and start killing threads (at this point, their work doesn't matter anymore
	return result;
}