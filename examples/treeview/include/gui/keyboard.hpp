#ifndef KEYBOARD_HPP
    #define KEYBOARD_HPP

    #include <functional>
    
    #include <SDL2/SDL.h>

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

    inline int operator|(Mod lhs, Mod rhs) {
        return (int)lhs|(int)rhs;
    }

    inline int operator|(int lhs, Mod rhs) {
        return lhs|(int)rhs;
    }

    inline int operator&(Mod lhs, Mod rhs) {
        return (int)lhs&(int)rhs;
    }

    inline int operator&(int lhs, Mod rhs) {
        return lhs&(int)rhs;
    }

    struct KeyboardShortcut {
        int key;
        Mod ctrl;
        Mod shift;
        Mod alt;
        Mod gui;
        int modifiers;
        std::function<void()> callback;

        KeyboardShortcut(int key, Mod ctrl, Mod shift, Mod alt, Mod gui, int modifiers, std::function<void()> callback) {
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