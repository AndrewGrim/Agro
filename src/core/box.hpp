#ifndef BOX_HPP
    #define BOX_HPP

    template <typename T> struct Box {
        T *ptr;

        Box() : ptr{nullptr} {}

        Box(T *ptr) : ptr{ptr} {}

        Box(Box &&box) noexcept {
            T *temp = box.ptr;
            box.ptr = ptr;
            ptr = temp;
        }

        Box(const Box &box) = delete;

        ~Box() {
            delete ptr;
        }

        T& operator*() {
            return *ptr;
        }

        T* operator->() {
            return ptr;
        }

        operator bool() {
            return ptr;
        }

        Box& operator=(const Box &box) = delete;

        Box& operator=(Box &&box) noexcept {
            T *temp = box.ptr;
            box.ptr = ptr;
            ptr = temp;
        }
    };
#endif
