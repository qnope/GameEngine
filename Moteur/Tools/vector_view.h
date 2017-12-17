#pragma once
#include <cassert>

template<typename T>
class vector_view {
public:
    vector_view(T *ptr, std::size_t capacity) : mPtr(ptr), mCapacity(capacity) {}

    void clear() {
        mSize = 0;
    }

    T* begin() {
        return mPtr;
    }

    T *end() {
        return mPtr + mSize;
    }

    std::size_t size() const {
        return mSize;
    }

    T* data() const {
        return mPtr;
    }

    T &operator[](uint32_t index) {
        assert(index < mSize);
        return (mPtr[index]);
    }

    void push_back(T const &v) {
        assert(mSize != mCapacity);
        mPtr[mSize++] = v;
    }

private:
    T *mPtr;
    std::size_t mSize;
    std::size_t mCapacity;
};
