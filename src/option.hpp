#ifndef OPTION_HPP
    #define OPTION_HPP

    template <typename T> struct Option {
        enum class Type {
            Some,
            None
        };

        Type type;
        T value;

        Option(T value) {
            this->type = Type::Some;
            this->value = value;
        }

        Option() {
            this->type = Type::None;
        }

        T unwrap() {
            assert(this->type == Type::Some);
            return this->value;
        }

        operator bool() {
            if (this->type == Type::Some) {
                return true;
            }
            return false;
        }
    };
#endif
