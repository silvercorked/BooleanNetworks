
export module Statistics:Mean;

import PrimitiveTypes;
import :InternalUtilities;

import <ranges>;
import <cmath>;

export namespace Stats {
	template <NumericRange Range>
	auto arithmeticMean(Range&& values) -> f64 {
		f64 len = static_cast<f64>(values.size());
		if (len == 0) return 0.0;
		f64 sum = 0.0;
		for (auto const& val : values)
			sum += static_cast<f64>(val);
		return sum / len;
	}
	
	// size is inclusive of this new element.
	template <IsNumeric NumericType>
	auto arithmeticMeanOnline(u32 size, NumericType nextVal, f64 prevMean) -> f64 {
		return prevMean + ((prevMean + static_cast<f64>(nextVal)) / static_cast<f64>(size));
	}

	template <NumericRange Range>
	auto geometricMean(Range&& values) -> f64 {
		f64 len = static_cast<f64>(values.size());
		f64 product = 1;
		for (auto const& val : values)
			product *= std::pow( // technically u do the multiplcations first then do the exponent
				static_cast<f64>(val), 1.0 / len
			);
		return product;
	}

	template <NumericRange Range>
	auto harmonicMean(Range&& values) -> f64 {
		f64 len = static_cast<f64>(values.size());
		if (len == 0) return 0.0;
		f64 sum = 0;
		for (auto const& val : values)
			sum += 1.0 / val;
		return len / (1.0 / sum);
	}
};
