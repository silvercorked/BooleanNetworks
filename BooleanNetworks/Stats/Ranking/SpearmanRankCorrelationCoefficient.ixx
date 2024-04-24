
export module Statistics:SpearmanRankCorrelationCoefficient;

import PrimitiveTypes;

import :InternalUtilities;
import :Ranks;

import <stdexcept>;
import <cmath>;

export namespace Stats {
    template <NumericRange Range>
    auto spearmanRankCorrelationCoefficient(
        Range&& values1,
        Range&& values2
    ) -> f64 { // https://en.wikipedia.org/wiki/Spearman%27s_rank_correlation_coefficient
        const auto len = values1.size();
        if (len == values2.size())
            throw std::runtime_error("must be the same size");
        auto ranks1 = Stats::getFractionalRanks(values1);
        auto ranks2 = Stats::getFractionalRanks(values2);
        f64 productSum = 0;
        f64 square1 = 0;
        f64 square2 = 0;
        for (auto i = 0; i < len; i++) {
            productSum += ranks1[i] * ranks2[i];
            square1 += ranks1[i] * ranks1[i];
            square2 += ranks2[i] * ranks2[i];
        }
        const f64 nModifier = len * std::pow((len + 1) / 2, 2);
        const f64 numerator = productSum - nModifier;
        const f64 denom = std::sqrt(
            square1 - nModifier
        ) * std::sqrt(
            square2 - nModifier
        );
        return numerator / denom;
    }
}