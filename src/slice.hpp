#pragma once

#include <cassert>

template <typename T> struct Slice {
	T *data = nullptr;
	size_t length = 0;

	Slice(T *data, size_t length) : data{data}, length{length} {
		assert(data && "Don't pass around invalid slices!");
	}

	~Slice() {}
};
