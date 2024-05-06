
export module Statistics:MWWSignedRanksTStatistic;

import PrimitiveTypes;

import :InternalUtilities;
import :Ranks;

import <stdexcept>;
import <cmath>;

export namespace Stats {
    template <NumericRange Range>
    auto MWWSignedRanksTStatistic(
        Range&& values1,
        Range&& values2
    ) -> f64 {
        const auto len1 = values1.size();
        const auto len2 = values2.size();
        if (len1 != len2)
            throw std::runtime_error("must be the same size");
        auto all = std::vector<std::ranges::range_value_t<Range>>();
        all.reserve(len1 + len2);
        for (const auto& n : values1)
            all.push_back(n);
        for (const auto& n : values2)
            all.push_back(n);
        auto ranks = getFractionalRanks(all);
        const auto N = ranks.size();
        f64 rankSumLen1 = 0;
        f64 rankSumSquared = 0;
        for (auto i = 0; i < N; i++) {
            if (i < len1) rankSumLen1 += ranks[i];
            rankSumSquared += ranks[i] * ranks[i];
        }
        const f64 numerator = rankSumLen1 - (static_cast<f64>(len1 * (2 * len1 + 1)) / 2);
        const f64 leftDenom = ((len1 * len1)
            / static_cast<f64>(2 * len1 * (2 * len1 - 1)))
            * rankSumSquared;
        const f64 rightDenom = ((len1 * len1 * std::pow(2 * len1 + 1, 2))
            / static_cast<f64>(4 * (2 * len1 - 1)));
        const f64 T = numerator
            / std::sqrt(
                leftDenom - rightDenom
            );
        return T;
    }
}