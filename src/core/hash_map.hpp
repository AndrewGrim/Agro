#ifndef HASH_MAP_HPP
    #define HASH_MAP_HPP

    #include <assert.h>

    #include "string.hpp"

    template <typename K> struct Hash {
        u32 operator()(K &key) const;
    };

    template <> struct Hash<i32> {
        u32 operator()(i32 &key) const {
            u32 hash = 2166136261;
            for (u8 i = 0; i < sizeof(i32); i++) {
                hash ^= ((u8*)&key)[i];
                hash *= 16777619;
            }
            return hash;
        }
    };

    template <> struct Hash<String> {
        u32 operator()(String &key) const {
            u32 hash = 2166136261;
            for (u32 i = 0; i < key.size(); i++) {
                hash ^= key[i];
                hash *= 16777619;
            }
            return hash;
        }
    };

    #define HASH_MAP_MAX_LOAD 0.75f
    #define HASH_MAP_DEFAULT_CAPACITY 256

    template <typename K, typename V, typename H = Hash<K>> struct HashMap {
        struct Entry {
            enum Flags : u32 {
                NOT_NULL    = 0b0000,
                NULL_KEY    = 0b0001,
                NULL_VALUE  = 0b0010,
            };
            u32 flags = Entry::NULL_KEY | Entry::NULL_VALUE;
            K key;
            V value;

            operator bool() {
                return !this->flags;
            }
        };

        u64 length = 0;
        u64 capacity = HASH_MAP_DEFAULT_CAPACITY;
        Entry *entries = nullptr;

        HashMap() {
            entries = new Entry[HASH_MAP_DEFAULT_CAPACITY];
        }

        ~HashMap() {
            delete[] entries;
        }

        bool insert(K key, V value) {
            if (length + 1 > capacity * HASH_MAP_MAX_LOAD) {
                u64 new_capacity = capacity * 2;
                Entry *new_entries = new Entry[new_capacity];
                assert(new_entries && "Failed to grow HashMap!");
                length = 0;
                for (u64 i = 0; i < capacity; i++) {
                    Entry &entry = entries[i];
                    if (entry) {
                        length++;
                        u32 index = Hash<K>()(entry.key) % new_capacity;
                        while (true) {
                            Entry &new_entry = new_entries[index];
                            if (new_entry.flags & Entry::NULL_KEY) {
                                new_entry.flags = Entry::NOT_NULL;
                                new_entry.key = entry.key;
                                new_entry.value = entry.value;
                                break;
                            }
                            index = (index + 1) % new_capacity;
                        }
                    } // Otherwise its either a null or a tombstone which we dont need to move.
                }
                capacity = new_capacity;
                delete[] entries;
                entries = new_entries;
            }
            Entry &entry = find(key);
            bool is_new_key = entry.flags & Entry::NULL_KEY;
            if (is_new_key && entry.flags & Entry::NULL_VALUE) { length++; }
            entry.flags = Entry::NOT_NULL;
            entry.key = key;
            entry.value = value;
            return is_new_key;
        }

        Entry& find(K key) {
            u32 index = Hash<K>()(key) % capacity;
            Entry *tombstone = nullptr;
            while (true) {
                Entry *entry = &entries[index];
                if (entry->flags & Entry::NULL_KEY) {
                    if (entry->flags & Entry::NULL_VALUE) {
                        return tombstone ? *tombstone : *entry;
                    } else {
                        if (!tombstone) { tombstone = entry; }
                    }
                } else if (entry->key == key) { // TODO here we could use a predicate similar to the generic hash
                    return *entry;
                }
                index = (index + 1) % capacity;
            }
        }

        Entry& operator[](K key) {
            return find(key);
        }

        bool remove(K key) {
            Entry &entry = find(key);
            if (entry.flags & Entry::NULL_KEY) { return false; }
            // We dont set Entry::NULL_VALUE here to make it a tombstone.
            entry.flags = Entry::NULL_KEY;
            // Set the key and value to defaults to avoid holding onto specific resources;
            entry.key = K();
            entry.value = V();
            return true;
        }
    };
#endif
