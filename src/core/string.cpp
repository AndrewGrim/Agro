#include "string.hpp"
#include "intrinsics.hpp"

namespace utf8 {
    u8 length(const char *_first_byte) {
        u8 first_byte = *_first_byte;
        if (first_byte <= 0b01111111) { return 1; }
        if (first_byte >= 0b11000000 && first_byte <= 0b11011111) { return 2; }
        if (first_byte >= 0b11100000 && first_byte <= 0b11101111) { return 3; }
        if (first_byte >= 0b11110000 && first_byte <= 0b11110111) { return 4; }
        return 0; // Invalid first byte.
    }

    u8 length(u32 codepoint) {
        if (codepoint < 0x80) { return 1; }
        if (codepoint < 0x800) { return 2; }
        if (codepoint < 0x10000) { return 3; }
        if (codepoint < 0x110000) { return 4; }
        assert(false && "codepoint too large");
        return 0;
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

    u8 encode(u32 codepoint, char *data) {
        u8 len = length(codepoint);
        switch (len) {
            case 1: {
                data[0] = cast(u8, codepoint);
                break;
            }
            case 2: {
                data[0] = cast(u8, 0b11000000 | (codepoint >> 6));
                data[1] = cast(u8, 0b10000000 | (codepoint & 0b111111));
                break;
            }
            case 3: {
                data[0] = cast(u8, 0b11100000 | (codepoint >> 12));
                data[1] = cast(u8, 0b10000000 | ((codepoint >> 6) & 0b111111));
                data[2] = cast(u8, 0b10000000 | (codepoint & 0b111111));
                break;
            }
            case 4: {
                data[0] = cast(u8, 0b11110000 | (codepoint >> 18));
                data[1] = cast(u8, 0b10000000 | ((codepoint >> 12) & 0b111111));
                data[2] = cast(u8, 0b10000000 | ((codepoint >> 6) & 0b111111));
                data[3] = cast(u8, 0b10000000 | (codepoint & 0b111111));
                break;
            }
            default: assert(false && "codepoint too large and len is zero when encoding");
        }
        return len;
    }

    bool validate(const String &text) {
        for (u64 i = 0; i < text.size();) {
            u8 len = utf8::length(text.data() + i);
            if (!len) { return false; }
            i += len;
        }
        return true;
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

namespace utf16 {
    u8 length(const char *_first_byte) {
        u32 codepoint = endian() == Endian::Big ? byteSwap<u16>(*(u16*)_first_byte) : *(u16*)_first_byte;
        if (!codepoint) { return 0; }
        if ((codepoint & ~0x03ff) == 0xd800) {
            return 4;
        }
        return 2;
    }

    // TODO length from codepoint
    // TODO encode
    // TODO decode

    Iterator::Iterator(const char *data) : begin{data}, data{data} {}

    Iterator::Iterator(const char *data, u64 size) : begin{data}, data{data + size} {}

    Iterator Iterator::next() {
        u32 c0 = endian() == Endian::Big ? byteSwap<u16>(*(u16*)data) : *(u16*)data;
        if (!c0) { codepoint = 0; return *this; }
        data += 2;
        if ((c0 & ~0x03ff) == 0xd800) {
            u32 c1 = endian() == Endian::Big ? byteSwap<u16>(*(u16*)data) : *(u16*)data;
            data += 2;
            length = 4;
            codepoint = 0x10000 + (((c0 & 0x03ff) << 10) | (c1 & 0x03ff));
        } else {
            length = 2;
            codepoint = c0;
        }
        return *this;
    }

    // TODO implement prev
    Iterator Iterator::prev() {
        return *this;
    }

    Iterator::operator bool() {
        return codepoint;
    }
}

String::String() { _setContent(0, nullptr); }

String::String(const char *text) { _setContent(strlen(text), text); }

String::String(const char *text, u64 length) { _setContent(length, text); }

String::String(const wchar_t *text) {
    const wchar_t *_text = text;
    while (*(u16*)_text++);
    _setContent((_text - text) * 2 + 1, (const char*)text);
}

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

u8& String::operator[](u64 index) const {
    return ((u8*)data())[index];
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

String::operator Slice<const char>() const {
    return this->slice();
}

void String::_setContent(u64 new_size, const char *text) {
    if (new_size > SMALL_STRING_BUFFER) {
        _string._heap._data = new u8[new_size + 1];  // +1 to account for null terminator.
        assert(_string._heap._data && "Failed memory allocation for String!");
        // Set the first byte to null to ensure its valid utf8.
        _string._heap._data[0] = '\0';
        if (text) {
            memcpy(_string._heap._data, text, new_size);
        }
        _string._heap._size = new_size;
        _string._heap._data[_string._heap._size] = '\0';
        _string._heap._capacity = new_size;
        _string._small._is_heap = true;
    } else {
        // Set the first byte to null to ensure its valid utf8.
        _string._small._data[0] = '\0';
        if (text) {
            memcpy(_string._small._data, text, new_size);
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
    assert(utf8::length(data()) && "trying to insert in a middle of a utf8 codepoint!");
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

Slice<const char> String::operator()() const {
    return Slice<const char>(data(), size());
}

Slice<const char> String::operator()(u64 begin) const {
    return Slice<const char>(data() + begin, size() - begin);
}

Slice<const char> String::operator()(u64 begin, u64 end) const {
    return Slice<const char>(data() + begin, end - begin);
}

Option<u64> String::find(const String &query) const {
    return findFirst(query);
}

Option<u64> String::findFirst(const String &query) const {
    u64 ssize = size();
    u64 qsize = query.size();
    if (qsize == 0) return Option<u64>(0);
    if (qsize > ssize) return Option<u64>();

    u64 skip_table[256];
    {
        for (u64 i = 0; i < 256; i++) {
            skip_table[i] = qsize;
        }
        for (u64 i = 0; i < qsize - 1; i++) {
            skip_table[query[i]] = qsize - 1 - i;
        }
    }

    {
        for (u64 i = 0; i <= ssize - qsize; i += skip_table[(*this)[i + qsize - 1]]) {
            if ((*this)(i, i + qsize) == query()) {
                return Option<u64>(i);
            }
        }
    }

    return Option<u64>();
}

Option<u64> String::findLast(const String &query) const {
    u64 ssize = size();
    u64 qsize = query.size();
    if (qsize == 0) return Option<u64>(ssize);
    if (qsize > ssize) return Option<u64>();

    u64 skip_table[256];
    {
        for (u64 i = 0; i < 256; i++) {
            skip_table[i] = qsize;
        }
        for (u64 i = qsize - 1; i > 0; i--) {
            skip_table[query[i]] = i;
        }
    }

    {
        for (u64 i = ssize - qsize; i <= 0; i += skip_table[(*this)[i + qsize - 1]]) {
            if ((*this)(i, i + qsize) == query()) {
                return Option<u64>(i);
            }
        }
        u64 i = ssize - qsize;
        while (true) {
            if ((*this)(i, i + qsize) == query()) {
                return Option<u64>(i);
            }
            const u64 skip = skip_table[(*this)[i]];
            if (skip > i) break;
            i -= skip;
        }
    }

    return Option<u64>();
}

u64 String::count(u8 c) const {
    u64 i = 0;
    u64 count = 0;
    u8 *ptr = (u8*)data();
    u64 length = size();
    #if SIMD_WIDTH > 0
        const simd::Vector<u8, SIMD_WIDTH> mask(c);
        while (i + SIMD_WIDTH < length) {
            count += __builtin_popcount(simd::Vector<u8, SIMD_WIDTH>(ptr + i) == mask);
            i += SIMD_WIDTH;
        }
    #endif
    while (i < length) {
        if (ptr[i] == c) { count++; }
        i++;
    }
    return count;
}

auto setSize = [](u64 new_size, String &self) {
    self._isSmall() ? self._string._small._size = new_size : self._string._heap._size = new_size;
};

String String::toUtf16Le() const {
    // Assume that we need twice the space for wide string
    // to prevent needlessly growing the buffer and an extra + 1
    // to have two null bytes at the end.
    String result = String(size() * 2 + 1);
    setSize(0, result);
    auto iter = utf8Begin();
    u64 index = 0;
    while ((iter = iter.next())) {
        if (iter.codepoint < 0x10000) {
            u16 c = cast(u16, iter.codepoint);
            c = endian() == Endian::Big ? byteSwap<u16>(c) : c;
            result.insert(index, (const char*)&c, sizeof(c));
            index += sizeof(c);
        } else {
            u16 high = cast(u16, (iter.codepoint - 0x10000) >> 10) + 0xD800;
            u16 low = cast(u16, iter.codepoint & 0x3FF) + 0xDC00;
            u16 out[2];
            out[0] = endian() == Endian::Big ? byteSwap<u16>(high) : high;
            out[1] = endian() == Endian::Big ? byteSwap<u16>(low) : low;
            result.insert(index, (const char*)&out, sizeof(out));
            index += sizeof(out);
        }
        setSize(index, result);
    }
    result.data()[result.size()] = '\0';
    result.data()[result.size() + 1] = '\0';
    return result;
}

String String::toUtf8() const {
    // Assume its all ascii for inital size.
    String result = String(size() / 2);
    setSize(0, result);
    auto iter = utf16::Iterator(data());
    u64 index = 0;
    while ((iter = iter.next())) {
        u8 data[4];
        u8 len = utf8::encode(iter.codepoint, (char*)data);
        result.insert(index, (const char*)data, len);
        index += len;
        setSize(index, result);
    }
    result.data()[result.size()] = '\0';
    return result;
}

ArrayList<String> String::split(u8 c) {
    ArrayList<String> array;
    usize begin = 0;
    usize i = 0;
    for (u8 byte : *this) {
        if (byte == c) {
            array.append(this->substring(begin, i));
            begin = i + 1;
        }
        i++;
    }
    if (begin != size()) { array.append(this->substring(begin, i)); }
    return array;
}

String toString(int value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%d", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
}

String toString(long value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%ld", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
}

String toString(long long value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%lld", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
}

String toString(unsigned value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%u", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
}

String toString(unsigned long value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%lu", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
}

String toString(unsigned long long value) {
    i64 buffer_size = sizeof(value) * 4;
    String s = String(buffer_size - 1);
    i64 actual_size = snprintf(s.data(), buffer_size, "%llu", value);
    if (s._isSmall()) {
        s._string._small._size = actual_size;
    } else {
        s._string._heap._size = actual_size;
    }
    return s;
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
