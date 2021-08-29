#ifndef RESULT_HPP
    #define RESULT_HPP

    template <typename ERROR_T, typename VALUE_T> struct Result {
        enum class Type {
            Ok,
            Error
        };

        Type type;

        union Value {
            VALUE_T ok;
            ERROR_T error;
        };

        Value value;

        Result(VALUE_T value) {
            this->type = Type::Ok;
            this->value.ok = value;
        }

        Result(ERROR_T error) {
            this->type = Type::Error;
            this->value.error = error;
        }

        Value unwrap() {
            assert(this->type == Type::Ok);
            return this->value;
        }

        operator bool() {
            if (this->type == Type::Ok) {
                return true;
            }
            return false;
        }
    };
#endif
