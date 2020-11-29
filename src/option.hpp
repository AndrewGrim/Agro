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
        switch (this->type) {
            case Type::Some:
                return this->value;
                break;
            case Type::None:
                assert(this->type == Type::Some);
                break;
        }
    }
};