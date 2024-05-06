
export module Statistics:WilcoxonSignedRankTest;

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
    ) -> f64 {
        const auto len = values1.size();
        if (len != values2.size())
            throw std::runtime_error("must be the same size");
        auto diffs = std::vector<std::ranges::range_value_t<Range>>();
        auto signs = std::vector<bool>(); // true -> pos, false -> neg
        for (auto i = 0; i < len; i++) {
            if (values1[i] == values2[i]) continue; // drop zeroes
            auto diff = values1[i] - values2[i];
            diffs.push_back(std::abs(diff));
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