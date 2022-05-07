#ifndef STRING_HPP
    #define STRING_HPP

    #include <string.h>
    #include <assert.h>

    #include "../common/number_types.h"
    #include "../slice.hpp"

    namespace utf8 {
        static const u32 TWO_BYTES    = 0b00011111;
        static const u32 THREE_BYTES  = 0b00001111;
        static const u32 FOUR_BYTES   = 0b00000111;
        static const u32 CONTINUATION = 0b00111111;

        u8 length(const char *_first_byte);

        u32 decode(const char *first_byte, u8 length);

        // Note: That neither constructor guarantees a valid starting point.
        // It is up to the callee to construct the Iterator using a valid sequence.
        struct Iterator {
            const char *begin = nullptr;
            // Note: That if you are using next() for iteration in order
            // to get the char* to the last codepoint you need to access it like so:
            // iter.data - iter.length
            const char *data = nullptr;
            u8 length = 0;
            u32 codepoint = 0;

            // Initialise iterator at data address.
            Iterator(const char *data);
            // Initialise iterator at data address with size offset.
            // This way you can start iterating from the end and use prev()
            // to iterate in reverse without having to iterate forward first.
            Iterator(const char *data, u64 size);
            Iterator next();
            Iterator prev();
            operator bool();
        };
    }

    struct HeapString {
        u8 *_data;
        u64 _size;
        u64 _capacity;
        u8 _padding[sizeof(void*)];
    };

    #define SMALL_STRING_BUFFER (sizeof(HeapString) - 3) // -1 for null terminator, -1 for _size, -1 for _is_heap

    struct SmallString {
        u8 _data[SMALL_STRING_BUFFER + 1]; // +1 to have space for null terminator.
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

        String();
        String(const char *text);
        String(const char *text, u64 length);
        String(u64 starting_size);
        String(const String &string);
        String(String &&string);
        ~String();
        char* data() const;
        u64 size() const;
        u64 capacity() const;
        String& operator=(const char *text);
        String& operator=(const String &string);
        String& operator=(String &&string);
        friend bool operator==(const String &lhs, const String &rhs);
        friend bool operator==(const String &lhs, const char *rhs);
        friend void operator+=(String &lhs, const char *rhs);
        friend String operator+(String &lhs, const char *rhs);
        friend String operator+(String &&lhs, const char *rhs);
        void _setContent(u64 new_size, const char *text);
        bool _isSmall() const;
        // Note: this method only works on ascii.
        String toLower();
        // Note: this method only works on ascii.
        String toUpper();
        static String repeat(const char *text, u64 count);
        bool startsWith(const char *text) const;
        bool endsWith(const char *text) const;
        String substring(u64 begin, u64 end) const;
        char* begin() const;
        char* end() const;
        utf8::Iterator utf8Begin() const;
        utf8::Iterator utf8End() const;
        void insert(u64 index, const char *text);
        void insert(u64 index, Slice<const char> text);
        void insert(u64 index, const char *text, u64 length);
        void erase(u64 index, u64 count);
        void clear();
        Slice<const char> slice() const;
    };
#endif
