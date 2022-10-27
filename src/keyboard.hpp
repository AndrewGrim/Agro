#ifndef KEYBOARD_HPP
    #define KEYBOARD_HPP

    #include <functional>

    #include <SDL.h>

    #include "common/number_types.h"
    #include "core/hash_map.hpp"

    // TODO add enum class for keys

    enum class Mod {
        None = KMOD_NONE,
        LShift = KMOD_LSHIFT,
        RShift = KMOD_RSHIFT,
        Shift = LShift|RShift,
        LCtrl = KMOD_LCTRL,
        RCtrl = KMOD_RCTRL,
        Ctrl = LCtrl|RCtrl,
        LAlt = KMOD_LALT,
        RAlt = KMOD_RALT,
        Alt = LAlt|RAlt,
        LGui = KMOD_LGUI,
        RGui = KMOD_RGUI,
        Gui = LGui|RGui,
        Num = KMOD_NUM,
        Caps = KMOD_CAPS,
        Mode = KMOD_MODE,
        Reserved = KMOD_RESERVED,
    };

    inline i32 operator|(Mod lhs, Mod rhs) {
        return (i32)lhs|(i32)rhs;
    }

    inline i32 operator|(i32 lhs, Mod rhs) {
        return lhs|(i32)rhs;
    }

    inline i32 operator&(Mod lhs, Mod rhs) {
        return (i32)lhs&(i32)rhs;
    }

    inline i32 operator&(i32 lhs, Mod rhs) {
        return lhs&(i32)rhs;
    }

    struct KeyboardShortcut {
        i32 key = -1;
        i32 modifiers = -1;

        KeyboardShortcut() {}
        KeyboardShortcut(i32 key, i32 modifiers) : key{key}, modifiers{modifiers} {}

        friend bool operator==(const KeyboardShortcut &lhs, const KeyboardShortcut &rhs) {
            return (lhs.key == rhs.key) && (lhs.modifiers == rhs.modifiers);
        }
    };

    template <> struct Hash<KeyboardShortcut> {
        u32 operator()(const KeyboardShortcut &key) const {
            u32 hash = 2166136261;
            for (u8 i = 0; i < sizeof(key.key); i++) {
                hash ^= ((u8*)&key)[i];
                hash *= 16777619;
            }
            for (u8 i = 0; i < sizeof(key.modifiers); i++) {
                hash ^= ((u8*)&key)[i];
                hash *= 16777619;
            }
            return hash;
        }
    };
#endif
