
export module Statistics:PearsonCorrelationCoefficient;

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
	auto populationPearsonCorrelationCoefficient(
		Range&& values1,
		Range&& values2,
		const f64 preCalculatedMean1 = NOT_GIVEN,
		const f64 preCalculatedMean2 = NOT_GIVEN,
		const f64 preCalculatedPopulationStdDev1 = NOT_GIVEN,
		const f64 preCalculatedPopulationStdDev2 = NOT_GIVEN
	) -> f64 {
		const auto len = values1.size();
		if (len != values2.size() || len == 0) return 0.0;
		const f64 mean1 = isGiven(preCalculatedMean1) ? preCalculatedMean1 : Stats::arithmeticMean(values1);
		const f64 mean2 = isGiven(preCalculatedMean2) ? preCalculatedMean2 : Stats::arithmeticMean(values2);
		const f64 popStdDev1 = isGiven(preCalculatedPopulationStdDev1) ? preCalculatedPopulationStdDev1 : Stats::populationStandardDeviation(values1, mean1);
		const f64 popStdDev2 = isGiven(preCalculatedPopulationStdDev2) ? preCalculatedPopulationStdDev2 : Stats::populationStandardDeviation(values2, mean2);
		return Stats::covariance(values1, values2, mean1, mean2)
			/ (popStdDev1 * popStdDev2);
	}
	template <NumericRange Range>
	auto samplePearsonCorrelationCoefficient(
		Range&& values1,
		Range&& values2,
		const f64 preCalculatedMean1 = NOT_GIVEN,
		const f64 preCalculatedMean2 = NOT_GIVEN
	) -> f64 {
		const auto len = values1.size();
		if (len != values2.size() || len == 0) return 0.0;
		const f64 mean1 = isGiven(preCalculatedMean1) ? preCalculatedMean1 : Stats::arithmeticMean(values1);
		const f64 mean2 = isGiven(preCalculatedMean2) ? preCalculatedMean2 : Stats::arithmeticMean(values2);
		f64 numerAccum = 0;
		f64 denomAccum1 = 0;
		f64 denomAccum2 = 0;
		for (auto i = 0; i < len; i++) {
			f64 t1 = static_cast<f64>(values1[i]) - mean1;
			f64 t2 = static_cast<f64>(values2[i]) - mean2;
			numerAccum += t1 * t2;
			denomAccum1 += std::pow(t1, 2);
			denomAccum2 += std::pow(t2, 2);
		}
		return numerAccum / (
			std::sqrt(denomAccum1) * std::sqrt(denomAccum2)
		);
	}
};
