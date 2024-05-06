
export module Statistics:MannWhitenyUStatistic;

import PrimitiveTypes;

import :InternalUtilities;
import :Ranks;

import <stdexcept>;
import <cmath>;

export namespace Stats {
    template <NumericRange Range>
    auto mannWhitenyUStatistic(
        Range&& values1,
        Range&& values2
    ) -> f64 {
        const auto len1 = values1.size(); // https://sphweb.bumc.bu.edu/otlt/MPH-Modules/BS/BS704_Nonparametric/BS704_Nonparametric4.html
        const auto len2 = values2.size();
        if (len1 != len2)
            throw std::runtime_error("must be the same size");
        auto all = std::vector<std::ranges::range_value_t<Range>>();
        all.reserve(len1 + len2);
        for (const auto& n : values1)
            all.push_back(n);
        for (const auto& n : values2)
            all.push_back(n);
        const auto ranks = getFractionalRanks(all);
        f64 sum1 = 0;
        f64 sum2 = 0;
        for (auto i = 0; i < len1; i++)
            sum1 += ranks.at(i);
        for (auto i = 0; i < len2; i++)
            sum2 += ranks.at(len1 + i);
        f64 u1 = len1 * len2 + ((len1 * (len1 + 1)) / 2) - sum1;
        f64 u2 = len1 * len2 + ((len2 * (len2 + 1)) / 2) - sum2;
        //assert(u1 + u2 == len1 * len2); // property of U statistics given ranking properties
        if (u1 < u2) return u1; // want smaller of 2
        else return u2;
    }
    // lower value (closer to zero -> reserach hypothesis)
    // higher value (closer to max (len1 * len2) / 2 -> null hypothesis
}