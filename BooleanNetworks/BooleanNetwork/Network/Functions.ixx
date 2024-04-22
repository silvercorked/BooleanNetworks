
export module BooleanNetwork:Functions;

import PrimitiveTypes;

import <vector>;

import :Node;

export namespace NodeFunctions {
	const auto AND_2 = FunctionType(
		[](const std::vector<bool>& bools) -> bool {
			return bools.at(0) && bools.at(1);
		}
	);
	const auto AND_n = FunctionType(
		[](const std::vector<bool>& bools) -> bool {
			bool all = true;
			for (const auto& elem : bools)
				all &= elem;
			return all;
		}
	);
    const auto OR_2 = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            return bools.at(0) || bools.at(1);
        }
    );
    const auto OR_n = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            bool all = true;
            for (const auto& elem : bools)
                all |= elem;
            return all;
        }
    );
    const auto NEGATE_1 = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            return !bools.at(0);
        }
    );
    const auto XOR_2 = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            return (bools.at(0) && !bools.at(1)) || (!bools.at(0) && bools.at(1));
        }
    );
    const auto XOR_n = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            bool all = true;
            for (const auto& elem : bools)
                all ^= elem;
            return all;
        }
    );
    const auto BUFFER_1 = FunctionType(
        [](const std::vector<bool>& bools) -> bool {
            return bools.at(0);
        }
    );
}
