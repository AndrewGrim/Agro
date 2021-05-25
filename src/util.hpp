#ifndef UTIL_HPP
    #define UTIL_HPP
    
        #include <iostream>
        #include <ostream>

        template <typename T> void pprint(int indent, T message, bool newline = true) {
            for (int i = 0; i < indent; i++) {
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

        void warn(std::string message, std::string text);
        void error(std::string message, std::string text);
#endif