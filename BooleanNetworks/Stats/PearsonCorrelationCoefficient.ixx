
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
		f64 len = static_cast<f64>(values1.size());
		if (values1.size() != values2.size()) return 0.0;
		f64 mean1 = isGiven(preCalculatedMean1) ? preCalculatedMean1 : Stats::arithmeticMean(values1);
		f64 mean2 = isGiven(preCalculatedMean2) ? preCalculatedMean2 : Stats::arithmeticMean(values2);
		f64 popStdDev1 = isGiven(preCalculatedPopulationStdDev1) ? preCalculatedPopulationStdDev1 : Stats::populationStandardDeviation(values1, mean1);
		f64 popStdDev2 = isGiven(preCalculatedPopulationStdDev2) ? preCalculatedPopulationStdDev2 : Stats::populationStandardDeviation(values2, mean2);
		return Stats::covariance(values1, values2, mean1, mean2)
			/ (popStdDev1 * popStdDev2);
	}
};
