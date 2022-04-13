#pragma once

#include <cassert>
#include "common/number_types.h"

template <typename T> struct Slice {
	T *data = nullptr;
	u64 length = 0;

	Slice(T *data, u64 length) : data{data}, length{length} {
		assert(data && "Don't pass around invalid slices!");
	}

	~Slice() {}
};
