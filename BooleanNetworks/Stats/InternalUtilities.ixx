
export module Statistics:InternalUtilities;

import PrimitiveTypes;

import <limits>;
import <vector>;
import <span>;
import <type_traits>;
import <ranges>;

export constexpr const f64 NOT_GIVEN = std::numeric_limits<f64>::lowest();

export auto isGiven(const f64 numToTest) -> bool {
	return numToTest != NOT_GIVEN;
}

export
template <typename NumericType>
concept IsNumeric = std::is_integral_v<NumericType> || std::is_floating_point_v<NumericType>;

export template <typename _Range>
concept NumericRange = std::ranges::input_range<_Range> && IsNumeric<std::ranges::range_value_t<_Range>>;
