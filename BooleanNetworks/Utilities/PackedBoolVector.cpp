
module PackedBoolVector;

import <stdexcept>;
import <bit>;

PackedBoolVector::PackedBoolVector(u32 size) : data() {
	if (size == 0)
		throw std::invalid_argument("Size cannot be zero!");
	u32 remainder = size % this->DataTypeSize;
	u32 nSize = remainder != 0
		? (size / this->DataTypeSize) + 1
		: (size / this->DataTypeSize);
	this->data.resize(nSize);
	this->dataSize = static_cast<u32>(this->data.size());
	this->offset = remainder != 0
		? remainder - 1
		: this->DataTypeSize - 1;
}
PackedBoolVector::~PackedBoolVector() {}

auto PackedBoolVector::size() const -> u32 {
	return ((static_cast<u32>(this->data.size()) - 1) * this->DataTypeSize) + offset + 1;
}
auto PackedBoolVector::at(u32 index) const -> bool {
	u32 nOffset = index % this->DataTypeSize;
	u32 nIndex = index / this->DataTypeSize;
	if (nIndex > this->data.size() || (nIndex == this->data.size() - 1 && nOffset > this->offset))
		throw std::out_of_range("Index out of range!");
	return this->data[nIndex] & (1 << nOffset);
}
auto PackedBoolVector::setTo(u32 index, bool value) -> void {
	u32 nOffset = index % this->DataTypeSize;
	u32 nIndex = index / this->DataTypeSize;
	if (nIndex > this->data.size() || (nIndex == this->data.size() - 1 && nOffset > this->offset))
		throw std::out_of_range("Index out of range!");
	this->data[nIndex] = (
		this->data[nIndex] & ~(1 << nOffset)
	) | ((value ? 1 : 0) << nOffset);
}
auto PackedBoolVector::popcount() const -> u32 {
	u32 sum = 0;
	for (u32 set : this->data) {
		sum += static_cast<u32>(std::popcount(set));
	}
	return sum;
}
auto PackedBoolVector::clear() -> void {
	for (u32 set : this->data)
		set = 0;
}

auto PackedBoolVector::equal(const PackedBoolVector& other) const -> bool {
	if (this->size() != other.size()) return false;
	for (i32 i = 0; i < this->size(); i++) {
		if (this->at(i) != other.at(i)) return false;
	}
	return true;
}
