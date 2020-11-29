template <typename T> struct Option {
    enum class Type {
        Some,
        None
    };
    
    Type type;
    T value;

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