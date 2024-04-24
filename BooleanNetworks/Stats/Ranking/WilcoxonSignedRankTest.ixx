
export module Statistics:SpearmanRankCorrelationCoefficient;

import PrimitiveTypes;

import :InternalUtilities;
import :Ranks;

import <stdexcept>;
import <cmath>;

export namespace Stats {
    template <NumericRange Range>
    auto wilcoxonSignedRankTest(
        Range&& values1,
        Range&& values2
    ) -> f64 { // https://en.wikipedia.org/wiki/Spearman%27s_rank_correlation_coefficient
        const auto len = nums1.size();
        assert(len == nums2.size());
        auto diffs = std::vector<V>();
        auto signs = std::vector<bool>(); // true -> pos, false -> neg
        for (auto i = 0; i < len; i++) {
            if (nums1[i] == nums2[i]) continue; // drop zeroes
            f64 diff = static_cast<i64>(nums1[i]) - static_cast<int64_t>(nums2[i]);
            diffs.push_back(abs(diff));
            signs.push_back((diff > 0)); // wont be equal cases
        }
        auto ranks = getFractionalRanks(diffs);
        const auto n = ranks.size();
        f64 sum = 0;
        f64 sumSquared = 0;
        for (auto i = 0; i < n; i++) {
            if (signs[i])
                sum += ranks[i];
            else
                sum -= ranks[i];
            sumSquared += ranks[i] * ranks[i]; // ignores signs cause square
        }
        const f64 denom = 1 / std::sqrt(sumSquared);
        const f64 T = sum * denom;
        return std::min(
            std::erfc(-(T + denom) / std::sqrt(2)) / 2,
            std::erfc((T - denom) / std::sqrt(2)) / 2
        );
    }
}