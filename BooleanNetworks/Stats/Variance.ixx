
export module Statistics:Variance;

import PrimitiveTypes;
import :InternalUtilities;
import :Mean;

import <ranges>;
import <cmath>;

template <NumericRange Range>
auto variance(Range&& values, const f64 preCalculatedMean = NOT_GIVEN, bool sample = true) -> f64 {
	f64 len = static_cast<f64>(values.size());
	if (len <= 1) return 0.0; // need at least 2 things for a variance/standard deviation
	const f64 mean = isGiven(preCalculatedMean) ? preCalculatedMean : Stats::arithmeticMean(values);
	f64 cumulative = 0.0;
	for (const auto val : values)
		cumulative += pow(static_cast<f64>(val) - mean, 2);
	return cumulative / (len - static_cast<f64>(sample ? 1 : 0));
}

export namespace Stats {
	template <NumericRange Range>
	auto sampleVariance(Range&& values, const f64 preCalculatedMean = NOT_GIVEN) -> f64 {
		return variance(values, preCalculatedMean, true);
	}

	template <NumericRange Range>
	auto sampleStandardDeviation(Range&& values, const f64 preCalculatedMean = NOT_GIVEN) -> f64 {
		return sqrt(variance(values, preCalculatedMean, true));
	}

	template <NumericRange Range>
	auto populationVariance(Range&& values, const f64 preCalculatedMean = NOT_GIVEN) -> f64 {
		return variance(values, preCalculatedMean, false);
	}

	template <NumericRange Range>
	auto populationStandardDeviation(Range&& values, const f64 preCalculatedMean = NOT_GIVEN) -> f64 {
		return sqrt(variance(values, preCalculatedMean, false));
	}

	template <NumericRange Range>
	auto covariance(Range&& values1, Range&& values2, const f64 preCalculatedMean1 = NOT_GIVEN, const f64 preCalculatedMean2 = NOT_GIVEN) -> f64 {
		f64 len = static_cast<f64>(values1.size());
		if (values1.size() != values2.size()) return 0.0; // cannot give covariance if different lengths
		f64 mean1 = isGiven(preCalculatedMean1) ? preCalculatedMean1 : Stats::arithmeticMean(values1);
		f64 mean2 = isGiven(preCalculatedMean2) ? preCalculatedMean2 : Stats::arithmeticMean(values2);
		f64 accum = 0;
		for (u32 i = 0; i < values1.size(); i++) {
			accum += (
				(static_cast<f64>(values1[i]) - mean1) + (static_cast<f64>(values2[i]) - mean2)
			) / len;
		}
		return accum;
	}

	template <IsNumeric NumericType>
	auto populationVarianceOnline( // https://math.stackexchange.com/questions/102978/incremental-computation-of-standard-deviation
		u32 size,
		NumericType nextElement,
		f64 prevMean = 0,
		f64 prevVariance = 0,
		f64 currMean = NOT_GIVEN,
		u32 degreesOfFreedom = 1
	) -> f64 {
		if (size == 0 || size == 1) return 0; // varaince is undefined if size < 2
		if (!isGiven(currMean))
			currMean = Stats::arithmeticMeanOnline(size, nextElement, prevMean);
		return (
			(size / (degreesOfFreedom + 2))
			+ (
				(size - 1) * std::pow(prevMean - currMean, 2)
			) + std::pow(nextElement - currMean, 2)
		) / (size - degreesOfFreedom);
	}

	template <IsNumeric NumericType>
	auto sampleVarianceOnline( // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
		u32 size,
		NumericType currElement,
		f64 prevMean,
		f64 prevVariance = 0
	) -> f64 {
		if (size == 0 || size == 1) return 0;
		return prevVariance + (std::pow(currElement - prevMean, 2) / size) - (prevVariance / (size - 1));
	}

	template <IsNumeric NumericType>
	class SampleVarianceOnlineManager {
		u32 size;	// num elems
		f64 mean;	// means of elems
		f64 m2;		// running difference between elems and mean
	public:
		SampleVarianceOnlineManager() : size(0), mean(0), m2(0) {};
		auto get() const -> f64 {
			if (this->size <= 1) return 0;
			return this->m2 / (this->size - 1); // unbiased sample variance
		}
		auto add(const NumericType& currElement) -> void {
			this->size++;
			f64 delta = currElement - this->mean;
			this->mean += delta / this->size;
			this->m2 += delta * (currElement - this->mean);
		}
	};
};
