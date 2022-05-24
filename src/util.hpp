#ifndef UTIL_HPP
    #define UTIL_HPP

        #include <iostream>
        #include <ostream>

        #include "core/string.hpp"
        #include "common/number_types.h"

        template <typename T> void pprint(i32 indent, T message, bool newline = true) {
            for (i32 i = 0; i < indent; i++) {
                std::cout << " ";
            }
            std::cout << message;
            if (newline) {
                std::cout << std::endl;
            }
        }

        template <typename T> void print(T message) {
            std::cout << message;
        }

        template <typename T> void println(T message) {
            std::cout << message << std::endl;
        }

        void info(String message, String text = "");
        void warn(String message, String text = "");
        void fail(String message, String text = "");
#endif
