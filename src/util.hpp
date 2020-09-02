#ifndef UTIL_HPP
    #define UTIL_HPP
        #include <iostream>

        template <typename T> void pprint(int indent, T message, bool newline = true) {
            for (int i = 0; i < indent; i++) {
                std::cout << " ";
            }
            std::cout << message;
            if (newline) std::cout << "\n";
        }

        template <typename T> void print(T message) {
            std::cout << message;
        }

        template <typename T> void println(T message) {
            std::cout << message << "\n";
        }
#endif