#ifndef AGRO_ARRAY_LIST_HPP
    #define AGRO_ARRAY_LIST_HPP

    #include <assert.h>
    #include <string.h>
    #include <type_traits>

    #include "../common/number_types.h"

    template <typename T> struct ArrayList {
        static_assert(
            std::is_default_constructible<T>::value and
            std::is_copy_constructible<T>::value and
            std::is_copy_assignable<T>::value,
            "T needs to be default constructible T() and be copy constructible T(const T&) and be copy assignable!"
        );

        T *__data = nullptr;
        usize __size = 0;
        usize __capacity = 0;

        ArrayList(usize capacity = 8) : __capacity{capacity} {
            __data = (T*)malloc(__capacity * sizeof(T));
        }

        ~ArrayList() {
            __destructElements();
            free(__data);
        }

        void append(T element) {
            __resize(__size + 1);
            new(__data + __size++) T(element);
        }

        T erase(usize index) {
            assert(index < __size and "Removing element out of bounds!");
            T element = __data[index];
            __data[index].~T();
            __move(__data + index, __data + (index + 1), __size - (index + 1));
            __size--;
            return element;
        }

        void erase(usize begin, usize end) {
            if (begin == end) { return; }
            usize count = end - begin;
            assert(begin + count <= __size);
            assert(begin < __size and end <= __size and end > begin and "Removing element out of bounds!");
            for (usize i = begin; i < end; i++) {
                __data[i].~T();
            }
            // TODO i feel like we should be able to optimise out those temporary allocations
            T *temp = (T*)malloc((__size - end) * sizeof(T));
            __move(temp, __data + end, __size - end);
            __move(__data + begin, temp, __size - end);
            free(temp);
            __size -= count;
        }

        T& operator[](usize index) const {
            assert(index < __size and "Accessing element out of bounds!");
            return __data[index];
        }

        void insert(usize index, T element) {
            assert(index <= __size and "Inserting element out of bounds!");
            __resize(__size + 1);
            // TODO i feel like we should be able to optimise out those temporary allocations
            T *temp = (T*)malloc((__size - index) * sizeof(T));
            __move(temp, __data + index, __size - index);
            __move(__data + (index + 1), temp, __size - index);
            free(temp);
            new(__data + index) T(element);
            __size++;
        }

        template <typename ...Targs> void emplace(usize index, Targs ...args) {
            assert(index <= __size and "Inserting element out of bounds!");
            __resize(__size + 1);
            // TODO i feel like we should be able to optimise out those temporary allocations
            T *temp = (T*)malloc((__size - index) * sizeof(T));
            __move(temp, __data + index, __size - index);
            __move(__data + (index + 1), temp, __size - index);
            free(temp);
            new(__data + index) T(args...);
            __size++;
        }

        void clear() {
            __destructElements();
            __size = 0;
        }

        void shiftLeft(usize from, usize count) {
            assert(from);
            assert(count);
            assert(from < __size and count < __size and from + count <= __size);
            T *temp = (T*)malloc(count * sizeof(T));
            __move(temp, __data + from, count);
            __move(__data + from + count - 1, __data + from - 1, 1);
            __move(__data + from - 1, temp, count);
            free(temp);
        }

        void shiftRight(usize from, usize count) {
            assert(count);
            assert(from + 1 < __size and count < __size and from + count + 1 <= __size);
            T *temp = (T*)malloc(count * sizeof(T));
            __move(temp, __data + from, count);
            __move(__data + from, __data + from + count, 1);
            __move(__data + from + 1, temp, count);
            free(temp);
        }

        T* data() const {
            return __data;
        }

        usize size() const {
            return __size;
        }

        usize capacity() const {
            return __capacity;
        }

        T* begin() const {
            return __data;
        }

        T* end() const {
            return __data + __size;
        }

        void __destructElements() {
            // Note that if T is a pointer type
            // then the user is responsible for freeing
            // what the pointers point to.
            for (usize i = 0; i < __size; i++) {
                __data[i].~T();
            }
        }

        void __resize(usize length) {
            if (length >= __capacity) {
                __capacity *= 2;
                T *temp = (T*)malloc(__capacity * sizeof(T));
                __move(temp, __data, __size);
                free(__data);
                __data = temp;
            }
        }

        void __move(T *to, T *from, usize count) {
            for (usize i = 0; i < count; i++) {
                new (to + i) T(from[i]);
                from[i].~T();
            }
        }
    };
#endif
