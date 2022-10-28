#ifndef RESULT_HPP
    #define RESULT_HPP

    template <typename E, typename V> struct Result {
        enum class Type {
            Ok,
            Error
        };

        Type type;
        E error;
        V value;

        Result(V &&value) {
            this->type = Type::Ok;
            this->value = std::move(value);
        }

        Result(E error) {
            this->type = Type::Error;
            this->error = error;
        }

        V unwrap() {
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
