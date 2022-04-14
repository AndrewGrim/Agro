#ifndef STRING_HPP
    #define STRING_HPP

    #include <string.h>
    #include <assert.h>

    #include "../common/number_types.h"

    namespace utf8 {
        static const u32 TWO_BYTES    = 0b00011111;
        static const u32 THREE_BYTES  = 0b00001111;
        static const u32 FOUR_BYTES   = 0b00000111;
        static const u32 CONTINUATION = 0b00111111;

        u8 length(const char *_first_byte) {
            u8 first_byte = *_first_byte;
            if (first_byte <= 0b01111111) { return 1; }
            if (first_byte >= 0b11000000 && first_byte <= 0b11011111) { return 2; }
            if (first_byte >= 0b11100000 && first_byte <= 0b11101111) { return 3; }
            if (first_byte >= 0b11110000 && first_byte <= 0b11110111) { return 4; }
            return 0; // Invalid first byte.
        }

        u32 decode(const char *first_byte, u8 length) {
            u32 codepoint = first_byte[0];
            switch (length) {
                case 1:
                    return codepoint;
                case 2:
                    codepoint &= utf8::TWO_BYTES;
                    codepoint <<= 6;
                    codepoint |= first_byte[1] & utf8::CONTINUATION;
                    return codepoint;
                case 3:
                    codepoint &= utf8::THREE_BYTES;
                    codepoint <<= 6;
                    codepoint |= first_byte[1] & utf8::CONTINUATION;
                    codepoint <<= 6;
                    codepoint |= first_byte[2] & utf8::CONTINUATION;
                    return codepoint;
                case 4:
                    codepoint &= utf8::FOUR_BYTES;
                    codepoint <<= 6;
                    codepoint |= first_byte[1] & utf8::CONTINUATION;
                    codepoint <<= 6;
                    codepoint |= first_byte[2] & utf8::CONTINUATION;
                    codepoint <<= 6;
                    codepoint |= first_byte[3] & utf8::CONTINUATION;
                    return codepoint;
                default:
                    return 0;
            }
        }

        // Note: That neither constructor guarantees a valid starting point.
        // It is up to the callee to construct the Iterator using a valid sequence.
        struct Iterator {
            char *begin = nullptr;
            char *data = nullptr;
            u8 length = 0;
            u32 codepoint = 0;

            // Initialise iterator at data address.
            Iterator(char *data) : begin{data}, data{data} {}

            // Initialise iterator at data address with size offset.
            // This way you can start iterating from the end and use prev()
            // to iterate in reverse without having to iterate forward first.
            Iterator(char *data, u64 size) : begin{data}, data{data + size} {}

            Iterator next() {
                length = utf8::length(data);
                codepoint = utf8::decode(data, length);
                if (codepoint) { data += length; }
                return *this;
            }

            Iterator prev() {
                if (begin == data) {
                    codepoint = 0;
                    return *this;
                }
                while (!(length = utf8::length(--data)));
                codepoint = utf8::decode(data, length);
                return *this;
            }

            operator bool() {
                return codepoint;
            }
        };
    }

    struct HeapString {
        char *_data;
        u64 _size;
        u64 _capacity;
        u8 _padding[sizeof(void*)];
    };

    #define SMALL_STRING_BUFFER (sizeof(HeapString) - 3) // -1 for null terminator, -1 for _size, -1 for _is_heap

    struct SmallString {
        char _data[SMALL_STRING_BUFFER];
        u8 _size;
        bool _is_heap;
        // We can infer capacity based on the fact that its a small string.
    };

    union StringData {
        HeapString _heap;
        SmallString _small;
    };

    struct String {
        StringData _string;

        String(const char *text) { _setContent(strlen(text), text); }

        String(u64 starting_size) { _setContent(starting_size, ""); }

        String(String &string) {
            this->~String();
            _setContent(string.size(), string.data());
        }

        String(String &&string) {
            this->~String();
            memcpy((void*)this, (void*)&string, sizeof(String));
            string._string._heap._data = nullptr;
        }

        ~String() {
            if (!_isSmall()) { delete[] _string._heap._data; }
        }

        char* data() const {
            return _isSmall() ? (char*)_string._small._data : (char*)_string._heap._data;
        }

        size_t size() const {
            return _isSmall() ? _string._small._size : _string._heap._size;
        }

        size_t capacity() const {
            return _isSmall() ? SMALL_STRING_BUFFER : _string._heap._capacity;
        }

        String& operator=(const char *text) {
            this->~String();
            _setContent(strlen(text), text);
            return *this;
        }

        String& operator=(String &&string) {
            this->~String();
            memcpy((void*)this, (void*)&string, sizeof(String));
            string._string._heap._data = nullptr;
            return *this;
        }

        friend bool operator==(const String &lhs, const String &rhs) {
            if (lhs.size() != rhs.size()) { return false; }
            return memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
        }

        friend bool operator==(const String &lhs, const char *rhs) {
            u64 length = strlen(rhs);
            if (lhs.size() != length) { return false; }
            return memcmp(lhs.data(), rhs, lhs.size()) == 0;
        }

        void _setContent(size_t new_size, const char *text) {
            if (new_size > SMALL_STRING_BUFFER) {
                _string._heap._data = new char[new_size + 1];  // +1 to account for null terminator.
                assert(_string._heap._data && "Failed memory allocation for String!");
                strcpy(_string._heap._data, text);
                _string._heap._size = new_size;
                _string._heap._capacity = new_size;
                _string._small._is_heap = true;
            } else {
                strcpy(_string._small._data, text);
                _string._small._size = new_size;
                _string._small._is_heap = false;
            }
        }

        bool _isSmall() const {
            return !_string._small._is_heap;
        }

        // Note: this method only works on ascii.
        String toLower() {
            for (u64 i = 0; i < size(); i++) {
                u8 byte = data()[i];
                if (byte > 64 && byte < 91) {
                    data()[i] = byte + 32;
                }
            }
            return *this;
        }

        // Note: this method only works on ascii.
        String toUpper() {
            for (u64 i = 0; i < size(); i++) {
                u8 byte = data()[i];
                if (byte > 96 && byte < 123) {
                    data()[i] = byte - 32;
                }
            }
            return *this;
        }

        static String repeat(const char *text, u64 count) {
            u64 length = strlen(text);
            String s = String(length * count);
            for (u64 i = 0; i < count; i++) {
                memcpy(s.data() + (length * i), text, length);
            }
            s.data()[s.size()] = '\0';
            return s;
        }

        bool startsWith(const char *text) {
            u64 length = strlen(text);
            if (size() < length) { return false; }
            return memcmp(data(), text, length) == 0;
        }

        bool endsWith(const char *text) {
            u64 length = strlen(text);
            if (size() < length) { return false; }
            return memcmp(data() + size() - length, text, length) == 0;
        }

        String substring(u64 begin, u64 end) {
            // TODO ideally we will use option in this case
            // another option is to silently just substring until end of data instead
            assert(size() - begin >= end - begin);
            String s = String(end - begin);
            memcpy(s.data(), data() + begin, end - begin);
            s.data()[s.size()] = '\0';
            return s;
        }

        char* begin() {
            return data();
        }

        char* end() {
            return data() + size();
        }

        utf8::Iterator utf8() {
            return utf8::Iterator(data());
        }

        utf8::Iterator utf8End() {
            return utf8::Iterator(data(), size());
        }

        void insert(u64 index, const char *text) {
            u64 length = strlen(text);
            u64 new_size = size() + length;
            if (new_size <= capacity()) {
                memcpy(data() + index + length, data() + index, size() - index);
                memcpy(data() + index, text, length);
                data()[new_size] = '\0';
                _isSmall() ? _string._small._size = new_size : _string._heap._size = new_size;
            } else {
                char *new_buffer = new char[new_size + 1];
                assert(new_buffer && "Failed memory allocation when inserting into String!");
                memcpy(new_buffer, data(), index);
                memcpy(new_buffer + index + length, data() + index, size() - index);
                memcpy(new_buffer + index, text, length);
                new_buffer[new_size] = '\0';
                if (!_isSmall()) {
                    delete _string._heap._data;
                } else {
                    _string._small._is_heap = true;
                }
                _string._heap._data = new_buffer;
                _string._heap._size = new_size;
                _string._heap._capacity = new_size;
            }
        }
    };
#endif
