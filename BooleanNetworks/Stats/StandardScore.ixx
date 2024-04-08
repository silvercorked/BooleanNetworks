
export module Statistics:StandardScore;

import PrimitiveTypes;
import :InternalUtilities;

export namespace Stats {
	template <IsNumeric X>
	auto standardScore(X x, const f64 mean, const f64 standardDeviation) -> f64 { // aka Z-score
		return (static_cast<f64>(x) - mean) / standardDeviation; // for samples/population respectively, mean and standard deviation must also match for right output
	} // ie, cant get correct z-score from x, sample mean, and population std dev
};