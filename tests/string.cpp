#include <stdio.h>

#include "../src/core/string.hpp"

int main() {
    // Note: all the following assumes utf8 encoded file.
    assert(sizeof(String) == 32);
    {
        String s = "abcdefghijklmnopqrstuvwxyz123";
        assert(s._isSmall());
        s = "abcdefghijklmnopqrstuvwxyz1234";
        assert(!s._isSmall());
        s = String("Test");
        assert(s._isSmall());
        String x = s;
        assert(x._isSmall());
        String z = (String&&)x;
        assert(z._isSmall());
    }
    {
        auto s1 = String("Óreiða");
        String s2 = "Óreiða";
        assert(s1.size() == s2.size());
    }
    {
        String s = "abc";
        assert(s.toUpper() == "ABC");
        assert(s.toLower() == "abc");
    }
    {
        String s1 = String::repeat("abc", 3);
        String s2 = String("abcabcabc");
        assert(s1 == "abcabcabc");
        assert(s1 == s2);
    }
    {
        String s = "abc123";
        assert(s.startsWith("abc"));
        assert(s.endsWith("123"));
    }
    {
        String s1 = "abc123";
        String s2 = s1.substring(3, 6);
        assert(s2 == "123");
        String s3 = s1.substring(0, 3);
        assert(s3 == "abc");
    }
    {
        {
            String s = "13";
            assert(s.size() == 2);
            assert(s._isSmall());
            s.insert(1, "2");
            assert(s.size() == 3);
            assert(s._isSmall());
            assert(s == "123");
        }
        {
            String s = "abcdefghijklmnopqrstuvwxyz123";
            assert(s.size() == 29);
            assert(s._isSmall());
            s.insert(3, "C");
            assert(s.size() == 30);
            assert(!s._isSmall());
            assert(s == "abcCdefghijklmnopqrstuvwxyz123");
        }
        {
            String s = "The quick brown fox jumped over the lazy dog.";
            assert(s.size() == 45);
            assert(!s._isSmall());
            s.insert(19, " 🦊");
            assert(s.size() == 50);
            assert(!s._isSmall());
            assert(s == "The quick brown fox 🦊 jumped over the lazy dog.");
        }
    }
    {
        {
            String s = "test";
            s.clear();
            assert(s.size() == 0);
            assert(s.capacity() == 29);
            assert(s == "");
        }
        {
            String s = "abcdefghijklmnopqrstuvwxyz1234";
            assert(!s._isSmall());
            assert(s.size() == 30);
            assert(s.capacity() == 30);
            s.clear();
            assert(s._isSmall());
            assert(s.size() == 0);
            assert(s.capacity() == 29);
            assert(s == "");
        }
    }
    {
        {
            String s = "abc";
            s += "123";
            assert(s == "abc123");
        }
        {
            String s = String("abc");
            assert(s + "123" == "abc123");
            assert(s == "abc");
            assert(s + "123" + "def" + "456" == "abc123def456");
        }
        {
            String s = String("abc") + "123";
            assert(s == "abc123");
        }

    }
    {
        String s = "abc";
        assert(s == s.slice().data);
        assert(s.slice().length == 3);
    }
    {
        String s = "";

        s = "a";
        assert(utf8::length(s.data()) == 1);
        assert((u8)s.data()[0] == 97);
        assert((u8)s.data()[1] == '\0');
        assert(utf8::decode(s.data(), utf8::length(s.data())) == 97);

        s = "ð";
        assert(utf8::length(s.data()) == 2);
        assert((u8)s.data()[0] == 195);
        assert((u8)s.data()[1] == 176);
        assert((u8)s.data()[2] == '\0');
        assert(utf8::decode(s.data(), utf8::length(s.data())) == 240);

        s = "⋒";
        assert(utf8::length(s.data()) == 3);
        assert((u8)s.data()[0] == 226);
        assert((u8)s.data()[1] == 139);
        assert((u8)s.data()[2] == 146);
        assert((u8)s.data()[3] == '\0');
        assert(utf8::decode(s.data(), utf8::length(s.data())) == 8914);

        s = "😁";
        assert(utf8::length(s.data()) == 4);
        assert((u8)s.data()[0] == 240);
        assert((u8)s.data()[1] == 159);
        assert((u8)s.data()[2] == 152);
        assert((u8)s.data()[3] == 129);
        assert((u8)s.data()[4] == '\0');
        assert(utf8::decode(s.data(), utf8::length(s.data())) == 128513);

        s = "Óreiða";
        assert(utf8::length(&s.data()[0]) == 2); // 'Ó'
        assert(utf8::length(&s.data()[1]) == 0); // second byte of 'Ó'
        assert(utf8::length(&s.data()[2]) == 1); // 'r'
        assert(utf8::length(&s.data()[3]) == 1); // 'e'
        assert(utf8::length(&s.data()[4]) == 1); // 'i'
        assert(utf8::length(&s.data()[5]) == 2); // 'ð'
        assert(utf8::length(&s.data()[6]) == 0); // second byte of 'ð'
        assert(utf8::length(&s.data()[7]) == 1); // 'a'
        assert(utf8::length(&s.data()[8]) == 1); // null terminator

        s = "lol 😂";
        assert(utf8::length(&s.data()[0]) == 1); // 'l'
        assert(utf8::length(&s.data()[1]) == 1); // 'o'
        assert(utf8::length(&s.data()[2]) == 1); // 'l'
        assert(utf8::length(&s.data()[3]) == 1); // ' '
        assert(utf8::length(&s.data()[4]) == 4); // '😂'
        assert(utf8::length(&s.data()[5]) == 0); // second byte of '😂'
        assert(utf8::length(&s.data()[6]) == 0); // third byte of '😂'
        assert(utf8::length(&s.data()[7]) == 0); // fourth byte of '😂'
        assert(utf8::length(&s.data()[8]) == 1); // null terminator
    }
    {
        String s1 = String("lol 😂");
        {
            // Iterate byte by byte.
            for (char c : s1) {
                printf("%c ", c);
            }
            printf("\n");
            // Iterate using utf8 codepoints from the beginning.
            utf8::Iterator iter = s1.utf8Begin();
            while ((iter = iter.next())) {
                printf("%u ", iter.codepoint);
            }
            printf("\n");
            // Iterate backwards to go back to the beginning.
            while ((iter = iter.prev())) {
                printf("%u ", iter.codepoint);
            }
            printf("\n");
            // Iterate using utf8 codepoints from the end.
            iter = s1.utf8End();
            while ((iter = iter.prev())) {
                printf("%u ", iter.codepoint);
            }
            printf("\n");
        }
        {
            utf8::Iterator iter = s1.utf8Begin();
            assert(iter.prev().codepoint == 0);
            assert(iter.next().codepoint == 108);
            assert(iter.next().codepoint == 111);
            assert(iter.next().codepoint == 108);
            assert(iter.next().codepoint == 32);
            assert(iter.next().codepoint == 128514);
            assert(iter.next().codepoint == 0);

            assert(iter.next().codepoint == 0);
            assert(iter.prev().codepoint == 128514);
            assert(iter.prev().codepoint == 32);
            assert(iter.prev().codepoint == 108);
            assert(iter.prev().codepoint == 111);
            assert(iter.prev().codepoint == 108);
            assert(iter.prev().codepoint == 0);

            iter = s1.utf8End();
            assert(iter.next().codepoint == 0);
            assert(iter.prev().codepoint == 128514);
            assert(iter.prev().codepoint == 32);
            assert(iter.prev().codepoint == 108);
            assert(iter.prev().codepoint == 111);
            assert(iter.prev().codepoint == 108);
            assert(iter.prev().codepoint == 0);
        }
    }
    return 0;
}
