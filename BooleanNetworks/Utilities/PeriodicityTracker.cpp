
module PeriodicityTracker;

import <cassert>;

PeriodicityTracker::PeriodicityTracker(u32 depth) : depth(depth), prevIterations() {
	assert(depth != 0);
}

PeriodicityTracker::~PeriodicityTracker() {}

auto PeriodicityTracker::addSample(const PackedBoolVector& sample) -> bool {
	if (this->prevIterations.size() != 0)
		if (this->prevIterations[0].size() != sample.size())
			return false; // different sizes cant be compared

	if (this->prevIterations.size() != this->depth)
		this->prevIterations.push_back(sample);
	else {
		this->prevIterations.pop_front();
		this->prevIterations.push_back(sample);
	}

	return true;
}

auto PeriodicityTracker::isPeriodic() const -> bool { // compare latest vs all others
	i32 latestIndex = this->prevIterations.size() - 1;
	if (latestIndex <= 0) return false; // either empty or only 1 element so cant be periodic from just that
	for (i32 i = 0; i < latestIndex; i++) {
		if (this->prevIterations[latestIndex].equal(this->prevIterations[i]))
			return true; // if any the same, periodicity found
	}
	return false;
}
