#include "string.hpp"

namespace utf8 {
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

    // Initialise iterator at data address.
    Iterator::Iterator(const char *data) : begin{data}, data{data} {}

    // Initialise iterator at data address with size offset.
    // This way you can start iterating from the end and use prev()
    // to iterate in reverse without having to iterate forward first.
    Iterator::Iterator(const char *data, u64 size) : begin{data}, data{data + size} {}

    Iterator Iterator::next() {
        length = utf8::length(data);
        codepoint = utf8::decode(data, length);
        if (codepoint) { data += length; }
        return *this;
    }

    Iterator Iterator::prev() {
        if (begin == data) {
            codepoint = 0;
            return *this;
        }
        while (!(length = utf8::length(--data)));
        codepoint = utf8::decode(data, length);
        return *this;
    }

    Iterator::operator bool() {
        return codepoint;
    }

}

String::String() { _setContent(0, nullptr); }

String::String(const char *text) { _setContent(strlen(text), text); }

String::String(const char *text, u64 length) { _setContent(length, text); }

String::String(u64 starting_size) { _setContent(starting_size, nullptr); }

String::String(const String &string) {
    _setContent(string.size(), string.data());
}

String::String(String &&string) {
    memcpy(this, &string, sizeof(String));
    string._string._heap._data = nullptr;
}

String::~String() {
    if (!_isSmall()) { delete[] _string._heap._data; }
}

char* String::data() const {
    return _isSmall() ? (char*)_string._small._data : (char*)_string._heap._data;
}

u64 String::size() const {
    return _isSmall() ? _string._small._size : _string._heap._size;
}

u64 String::capacity() const {
    return _isSmall() ? SMALL_STRING_BUFFER : _string._heap._capacity;
}

String& String::operator=(const char *text) {
    this->~String();
    _setContent(strlen(text), text);
    return *this;
}

String& String::operator=(const String &string) {
    this->~String();
    _setContent(string.size(), string.data());
    return *this;
}

String& String::operator=(String &&string) {
    this->~String();
    memcpy(this, &string, sizeof(String));
    string._string._heap._data = nullptr;
    return *this;
}

bool operator==(const String &lhs, const String &rhs) {
    if (lhs.size() != rhs.size()) { return false; }
    return memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

bool operator==(const String &lhs, const char *rhs) {
    u64 length = strlen(rhs);
    if (lhs.size() != length) { return false; }
    return memcmp(lhs.data(), rhs, lhs.size()) == 0;
}

void operator+=(String &lhs, const char *rhs) {
    lhs.insert(lhs.size(), rhs);
}

void operator+=(String &lhs, const String &rhs) {
    lhs.insert(lhs.size(), rhs.data());
}

String operator+(String &lhs, const char *rhs) {
    String new_string = lhs;
    new_string.insert(new_string.size(), rhs);
    return new_string;
}

String operator+(String &&lhs, const char *rhs) {
    String new_string = lhs;
    new_string.insert(new_string.size(), rhs);
    return new_string;
}

String operator+(String &lhs, const String &rhs) {
    String new_string = lhs;
    new_string.insert(new_string.size(), rhs.data());
    return new_string;
}

String operator+(String &&lhs, const String &rhs) {
    String new_string = lhs;
    new_string.insert(new_string.size(), rhs.data());
    return new_string;
}

void String::_setContent(u64 new_size, const char *text) {
    if (new_size > SMALL_STRING_BUFFER) {
        _string._heap._data = new u8[new_size + 1];  // +1 to account for null terminator.
        assert(_string._heap._data && "Failed memory allocation for String!");
        if (text) {
            memcpy(_string._heap._data, text, new_size);
        } else {
            _string._heap._data[0] = '\0';
        }
        _string._heap._size = new_size;
        _string._heap._data[_string._heap._size] = '\0';
        _string._heap._capacity = new_size;
        _string._small._is_heap = true;
    } else {
        if (text) {
            memcpy(_string._small._data, text, new_size);
        } else {
            _string._small._data[0] = '\0';
        }
        _string._small._size = new_size;
        _string._small._data[_string._small._size] = '\0';
        _string._small._is_heap = false;
    }
}

bool String::_isSmall() const {
    return !_string._small._is_heap;
}

// Note: this method only works on ascii.
String String::toLower() {
    for (u64 i = 0; i < size(); i++) {
        u8 byte = data()[i];
        if (byte > 64 && byte < 91) {
            data()[i] = byte + 32;
        }
    }
    return *this;
}

// Note: this method only works on ascii.
String String::toUpper() {
    for (u64 i = 0; i < size(); i++) {
        u8 byte = data()[i];
        if (byte > 96 && byte < 123) {
            data()[i] = byte - 32;
        }
    }
    return *this;
}

String String::repeat(const char *text, u64 count) {
    u64 length = strlen(text);
    String s = String(length * count);
    for (u64 i = 0; i < count; i++) {
        memcpy(s.data() + (length * i), text, length);
    }
    s.data()[s.size()] = '\0';
    return s;
}

String String::repeat(char text, u64 count) {
    u64 length = 1;
    String s = String(length * count);
    for (u64 i = 0; i < count; i++) {
        s.data()[i] = text;
    }
    s.data()[s.size()] = '\0';
    return s;
}

bool String::startsWith(const char *text) const {
    u64 length = strlen(text);
    if (size() < length) { return false; }
    return memcmp(data(), text, length) == 0;
}

bool String::endsWith(const char *text) const {
    u64 length = strlen(text);
    if (size() < length) { return false; }
    return memcmp(data() + size() - length, text, length) == 0;
}

String String::substring(u64 begin, u64 end) const {
    // TODO change to option
    assert(size() - begin >= end - begin);
    String s = String(end - begin);
    memcpy(s.data(), data() + begin, end - begin);
    s.data()[s.size()] = '\0';
    return s;
}

char* String::begin() const {
    return data();
}

char* String::end() const {
    return data() + size();
}

utf8::Iterator String::utf8Begin() const {
    return utf8::Iterator(data());
}

utf8::Iterator String::utf8End() const {
    return utf8::Iterator(data(), size());
}

void String::insert(u64 index, const char *text) {
    insert(index, text, strlen(text));
}

void String::insert(u64 index, Slice<const char> text) {
    insert(index, text.data, text.length);
}

void String::insert(u64 index, const char *text, u64 length) {
    // TODO change to result
    assert(index <= size());
    assert(utf8::length(data() + index));
    u64 new_size = size() + length;
    if (new_size <= capacity()) {
        memcpy(data() + index + length, data() + index, size() - index);
        memcpy(data() + index, text, length);
        data()[new_size] = '\0';
        _isSmall() ? _string._small._size = new_size : _string._heap._size = new_size;
    } else {
        u8 *new_buffer = new u8[new_size + 1];
        assert(new_buffer && "Failed memory allocation when inserting into String!");
        memcpy(new_buffer, data(), index);
        memcpy(new_buffer + index + length, data() + index, size() - index);
        memcpy(new_buffer + index, text, length);
        new_buffer[new_size] = '\0';
        if (!_isSmall()) { delete[] _string._heap._data; }
        else { _string._small._is_heap = true; }
        _string._heap._data = new_buffer;
        _string._heap._size = new_size;
        _string._heap._capacity = new_size;
    }
}

void String::erase(u64 index, u64 count) {
    // TODO change to result
    assert(index + count <= size());
    memcpy(data() + index, data() + index + count, size() - (index + count));
    _isSmall() ? _string._small._size -= count : _string._heap._size -= count;
    data()[size()] = '\0';
}

void String::clear() {
    if (!_isSmall()) { delete[] _string._heap._data; }
    _setContent(0, nullptr);
}

Slice<const char> String::slice() const {
    return Slice<const char>(data(), size());
}

Option<u64> String::find(const String &query) const {
    u8 first = query.data()[0];
    for (u64 i = 0; i < this->size(); i++) {
        if (this->data()[i] == first) {
            for (u64 j = 1; j < query.size(); j++) {
                if (this->data()[i + j] != query.data()[j]) {
                    goto NO_MATCH;
                }
            }
            return Option<u64>(i);
        }
        NO_MATCH:;
    }
    return Option<u64>();
}

String toString(int value) {
    return String::format("%d", value);
}

String toString(long value) {
    return String::format("%ld", value);
}

String toString(long long value) {
    return String::format("%lld", value);
}

String toString(unsigned value) {
    return String::format("%u", value);
}

String toString(unsigned long value) {
    return String::format("%lu", value);
}

String toString(unsigned long long value) {
    return String::format("%llu", value);
}

String toString(float value) {
    return String::format("%f", value);
}

String toString(double value) {
    return String::format("%f", value);
}

String toString(long double value) {
    return String::format("%Lf", value);
}
