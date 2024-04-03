module;

export module PackedBoolVector;

import PrimitiveTypes;

import <vector>;

export class PackedBoolVector {
	std::vector<u32> data;
	u32 dataSize;
	u32 offset;
	const u32 DataTypeSize = sizeof(u32) * 8;

public:
	PackedBoolVector(u32);
	~PackedBoolVector();

	auto size() const -> u32;
	auto at(u32) const -> bool;
	auto setTo(u32, bool) -> void;

	auto popcount() const -> u32;
	auto clear() -> void;
};