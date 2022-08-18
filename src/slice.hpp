#pragma once

#include <cassert>
#include "common/number_types.h"

template <typename T> struct Slice {
    T *data = nullptr;
    u64 length = 0;

    Slice() {}

    Slice(T *data) : data{data}, length{strlen(data)} {}

    Slice(T *data, u64 length) : data{data}, length{length} {
        assert(data && "Don't pass around invalid slices!");
    }

    ~Slice() {}
};

inline bool operator==(const Slice<const char> &lhs, const Slice<const char> &rhs) {
    if (lhs.length != rhs.length) return false;
    if (lhs.data == rhs.data) return true;
    for (u64 i = 0; i < lhs.length; i++) {
        if (lhs.data[i] != rhs.data[i]) return false;
    }
    return true;
}
