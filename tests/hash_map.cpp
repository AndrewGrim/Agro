#include <assert.h>

#include "../src/core/string.hpp"
#include "../src/core/hash_map.hpp"

int main() {
    {
        HashMap<String, String> map;
        for (char c = 'a'; c <= 'z'; c++) {
            map.insert(String::repeat(c, 1), String::repeat(c - 32, 1));
        }
        auto entry = map.find(String("f"));
        assert(!(entry.flags & HashMap<String, String>::Entry::NULL_KEY));
        assert(!(entry.flags & HashMap<String, String>::Entry::NULL_VALUE));
        assert(entry.value == String("F"));

        entry = map.find(String("%"));
        assert((entry.flags & HashMap<String, String>::Entry::NULL_KEY));
        assert((entry.flags & HashMap<String, String>::Entry::NULL_VALUE));
        assert(entry.value == String());
    }
    {
        HashMap<String, String> map;
        map.insert(String("jam"), String());
        map.insert(String("biscuit"), String());
        map.insert(String("bagel"), String());
        assert(map.length == 3);
        auto entry = map.find(String("jam"));
        assert(entry.value == String());
        map.insert(String("jam"), String("overwritten"));
        entry = map.find(String("jam"));
        assert(entry.value == String("overwritten"));
    }

    i32 n = 1337;
    assert(Hash<i32>()(n) == 3361957851);
    // Error about undefined symbol because there is no specialization for the u32 type. Expected behaviour.
    // u32 hash = Hash<u32>()(1);

    return 0;
}
