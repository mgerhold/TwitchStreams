#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <utility>

template<typename ElementType>
class COLA {
public:
	std::size_t size() const {
		return mNumElements;
	}

	std::size_t capacity() const {
		return mCapacity;
	}

	void insert(ElementType&& elementToInsert) {
		if (mCapacity < mNumElements + 1) {
			grow();
		}
		std::size_t n = mNumElements;
		std::vector<ElementType> overflow;
		overflow.push_back(std::move(elementToInsert));
		for (std::size_t level = 0; level < mData.size(); ++level) {
			if (!(n & 0b0001)) {
				mData.at(level) = std::move(overflow);
				overflow.clear();
				break;
			}
			std::size_t overflowSize = overflow.size();
			overflow.insert(overflow.end(),
				std::make_move_iterator(mData.at(level).begin()),
				std::make_move_iterator(mData.at(level).end()));
			mData.at(level).clear();
			std::inplace_merge(overflow.begin(),
				overflow.begin() + overflowSize,
				overflow.end());
			n >>= 1;
		}
		++mNumElements;
	}

	void insert(const ElementType& elementToInsert) {
		if (mCapacity < mNumElements + 1) {
			grow();
		}
		std::size_t n = mNumElements;
		std::vector<ElementType> overflow{ elementToInsert };
		for (std::size_t level = 0; level < mData.size(); ++level) {
			if (!(n & 0b0001)) {
				mData.at(level) = std::move(overflow);
				overflow.clear();
				break;
			}			
			std::size_t overflowSize = overflow.size();
			overflow.insert(overflow.end(),
				std::make_move_iterator(mData.at(level).begin()),
				std::make_move_iterator(mData.at(level).end()));
			mData.at(level).clear();
			std::inplace_merge(overflow.begin(),
				overflow.begin() + overflowSize,
				overflow.end());
			n >>= 1;
		}
		++mNumElements;
	}

	ElementType& find(const ElementType& needle) {
		std::size_t bitmask = 0b0000'0001LL << (mData.size() - 1);		
		std::size_t level = mData.size() - 1;
		while (bitmask) {
			if (bitmask & mNumElements) {
				auto it = std::lower_bound(
					mData.at(level).begin(),
					mData.at(level).end(),
					needle);
				if (it != mData.at(level).end() && *it == needle) {
					return *it;
				}
			}
			--level;
			bitmask >>= 1;
		}
		throw -1;
	}

private:
	void grow() {
		if (mCapacity == 0) {
			mData.emplace_back().reserve(1);
			mCapacity = 1;
			return;
		}
		std::size_t nextSize = mData.back().capacity() * 2;
		mData.emplace_back().reserve(nextSize);
		mCapacity += nextSize;
	}

private:
	std::vector<std::vector<ElementType>> mData;
	std::size_t mNumElements{ 0 };
	std::size_t mCapacity{ 0 };

	template<typename COLAElementType>
	friend std::ostream& operator<<(std::ostream& os, const COLA<COLAElementType>& cola);
};

template<typename COLAElementType>
std::ostream& operator<<(std::ostream& os, const COLA<COLAElementType>& cola) {
	if (cola.capacity() == 0) {
		os << "[empty]\n";
		return os;
	}
	for (const auto& arr : cola.mData) {
		os << "[";
		if (arr.size() == 0) {
			os << "null";
			for (std::size_t i = 1; i < arr.capacity(); ++i) {
				os << ", null";
			}
		} else {
			os << arr.at(0);
			for (std::size_t i = 1; i < arr.size(); ++i) {
				os << ", " << arr.at(i);
			}
		}
		os << "]\n";
	}
	return os;
}