#ifndef KEYBOARD_HPP
    #define KEYBOARD_HPP

    #include <functional>

    #include <SDL.h>

    #include "common/number_types.h"

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
        i32 key;
        Mod ctrl;
        Mod shift;
        Mod alt;
        Mod gui;
        i32 modifiers;
        std::function<void()> callback;

        KeyboardShortcut(i32 key, Mod ctrl, Mod shift, Mod alt, Mod gui, i32 modifiers, std::function<void()> callback) {
            this->key = key;
            this->ctrl = ctrl;
            this->shift = shift;
            this->alt = alt;
            this->gui = gui;
            this->modifiers = modifiers;
            this->callback = callback;
        }
    };
#endif
