module;

export module PackedBoolVector;

import PrimitiveTypes;

import <vector>;
import <type_traits>;

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

/*
// iterator functions
public:
	typedef PackedBoolVectorIterator<bool> iterator;
	typedef PackedBoolVectorIterator<const bool> const_iterator;

	typedef PackedBoolVectorReverseIterator<bool> reverse_iterator;
	typedef PackedBoolVectorReverseIterator<const bool> const_reverse_iterator;

	auto begin() -> iterator;
	auto end() -> iterator;

	auto cbegin() -> const_iterator;
	auto cend() -> const_iterator;

	auto rbegin() -> reverse_iterator;
	auto rend() -> reverse_iterator;

	auto crbegin() -> const_reverse_iterator;
	auto crend() -> const_reverse_iterator;
*/
};

/*
template <typename Type>
class PackedBoolVectorIterator {
protected:
	Type* m_ptr;
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = std::ptrdiff_t;
	using pointer = Type*;
	using reference = Type&;

	PackedBoolVectorIterator(Type* ptr) : m_ptr(ptr) {};
	PackedBoolVectorIterator(const PackedBoolVectorIterator<Type>& iterator) = default;
	~PackedBoolVectorIterator() = default;

	auto operator=(const PackedBoolVectorIterator<Type>& iterator) -> PackedBoolVectorIterator<Type>& = default;
	auto operator=(Type* ptr) -> PackedBoolVectorIterator<Type>& {
		this->m_ptr = ptr;
		return (*this);
	}
	auto operator bool() const -> bool {
		if (this->m_ptr) return true;
		return false;
	}

	auto operator==(const PackedBoolVectorIterator<Type>& iterator) const -> bool {
		return (this->m_ptr == iterator.getConstPtr());
	}
	auto operator!=(const PackedBoolVectorIterator<Type>& iterator) const -> bool {
		return (this->m_ptr != iterator.getConstPtr());
	}

	auto operator+=(const difference_type movement) -> PackedBoolVectorIterator<Type>& {
		this->m_ptr += movement;
		return (*this);
	}
	auto operator-=(const difference_type movement) -> PackedBoolVectorIterator<Type>& {
		this->m_ptr -= movement;
		return (*this);
	}
	auto operator++() -> PackedBoolVectorIterator<Type>& {
		++this->m_ptr;
		return (*this);
	}
	auto operator--() -> PackedBoolVectorIterator<Type>& {
		--this->m_ptr;
		return (*this);
	}

	auto operator++(int) -> PackedBoolVectorIterator<Type> {
		auto temp(*this);
		++this->m_ptr;
		return temp;
	}
	auto operator--(int) -> PackedBoolVectorIterator<Type> {
		auto temp(*this);
		--this->m_ptr;
		return temp;
	}
	auto operator+(const difference_type& movement) -> PackedBoolVectorIterator<Type> {
		auto oldPtr = this->m_ptr;
		this->m_ptr += movement;
		auto temp(*this);
		this->m_ptr = oldPtr;
		return temp;
	}
	auto operator-(const difference_type& movement) -> PackedBoolVectorIterator<Type> {
		auto oldPtr = this->m_ptr;
		this->m_ptr -= movement;
		auto temp(*this);
		this->m_ptr = oldPtr;
		return temp;
	}

	auto operator-(const PackedBoolVectorIterator<Type>& iterator) -> difference_type {
		return std::distance(iterator.getPtr(), this->getPtr());
	}

	auto operator*() -> Type& {
		return *this->m_ptr;
	}
	auto operator*() const -> const Type& {
		return *this->m_ptr;
	}
	auto operator->() -> Type* {
		return this->m_ptr;
	}

	auto getPtr() const -> Type* {
		return this->m_ptr;
	}
	auto getConstPtr() const -> const Type* {
		return this->m_ptr;
	}
};

static_assert(std::is_trivially_constructible_v<PackedBoolVectorIterator<bool>>);
static_assert(std::is_trivially_copyable_v<PackedBoolVectorIterator<bool>>);
static_assert(std::is_trivially_copy_assignable_v<PackedBoolVectorIterator<bool>>);
static_assert(std::is_trivially_destructible_v<PackedBoolVectorIterator<bool>>);
static_assert(std::is_trivial_v<PackedBoolVectorIterator<bool>>);
static_assert(std::input_or_output_iterator<PackedBoolVectorIterator<bool>>);
static_assert(std::sentinel_for<PackedBoolVectorIterator<bool>, PackedBoolVectorIterator<bool>>);
static_assert(std::incrementable<PackedBoolVectorIterator<bool>>);
static_assert(std::input_iterator<PackedBoolVectorIterator<bool>>);
static_assert(std::forward_iterator<PackedBoolVectorIterator<bool>>);
static_assert(std::bidirectional_iterator<PackedBoolVectorIterator<bool>>);
static_assert(std::totally_ordered<PackedBoolVectorIterator<bool>>);
static_assert(std::random_access_iterator<PackedBoolVectorIterator<bool>>);
*/
