#ifndef UTIL_HPP
    #define UTIL_HPP
        #include <iostream>

        template <typename T> void print(T message) {
            std::cout << message;
        }

        template <typename T> void println(T message) {
            std::cout << message << "\n";
        }
#endif