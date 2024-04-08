
export module Statistics:Skewness;

import PrimitiveTypes;
import :InternalUtilities;

import :Mean;
import :Variance;

import <cmath>;
import <ranges>;
import <algorithm>;
import <vector>;

export namespace Stats {
	template <NumericRange Range>
	auto populationSkewness(Range&& values, const f64 preCalculatedMean = NOT_GIVEN, const f64 preCalculatedPopulationStdDev = NOT_GIVEN) -> f64 { // this (attempts to) compute the "third standardized moment", mu3 // https://en.wikipedia.org/wiki/Skewness
		const f64 len = static_cast<f64>(values.size());
		if (len == 0) return 0.0;
		const f64 mean = isGiven(preCalculatedMean) ? preCalculatedMean : Stats::arithmeticMean(values);
		const f64 popStdDev = isGiven(preCalculatedPopulationStdDev) ? preCalculatedPopulationStdDev : Stats::populationStandardDeviation(values, mean);
		f64 accum = 0;
		for (const auto& val : values)
			accum += std::pow((static_cast<f64>(val) - mean) / popStdDev, 3);
		return accum / len;
	}
	template <NumericRange Range>
	auto sampleSkewness(Range&& values, const f64 preCalculatedMean = NOT_GIVEN, const f64 preCalculatedSampleStdDev = NOT_GIVEN) -> f64 { // https://en.wikipedia.org/wiki/Skewness#Sample_skewness
		const f64 len = static_cast<f64>(values.size());
		if (len == 0) return 0.0;
		const f64 mean = isGiven(preCalculatedMean) ? preCalculatedMean : Stats::arithmeticMean(values);
		const f64 sampleStdDev = isGiven(preCalculatedSampleStdDev) ?  preCalculatedSampleStdDev : Stats::sampleStandardDeviation(values, mean);
		f64 numer = 0;
		for (const auto& val : values) {
			f64 diffFromMean = static_cast<f64>(val) - mean;
			numer += std::pow(diffFromMean, 3);
		}
		return (numer * len) // https://www.spss-tutorials.com/skewness/
			/ (std::pow(sampleStdDev, 3.0) * (len - 1) * (len - 2));
	}
};
