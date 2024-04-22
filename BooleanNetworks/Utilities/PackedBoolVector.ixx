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
	~PackedBoolVector() = default;

	auto size() const -> u32;
	auto at(u32) const -> bool;
	auto setTo(u32, bool) -> void;

	auto popcount() const -> u32;
	auto clear() -> void;

	auto operator==(const PackedBoolVector& rhs) const -> bool {
		if (this->size() != rhs.size()) return false;
		for (i32 i = 0; i < this->underlyingSize(); i++) {
			if (this->getAtUnderlyingIndex(i) != rhs.getAtUnderlyingIndex(i))
				return false;
		}
		return true;
	}

private:
	auto getAtUnderlyingIndex(u32) const -> u32;
	auto underlyingSize() const -> i32;
};


