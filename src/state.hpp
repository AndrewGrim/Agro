#ifndef STATE_HPP
    #define STATE_HPP

    class Widget;

    struct State {
        Widget *soft_focused = nullptr;
        Widget *hard_focused = nullptr;
        Widget *focused = nullptr; // TODO remove, just here to ease the transition
        Widget *hovered = nullptr;
        Widget *pressed = nullptr;
        Widget *tooltip = nullptr;
    };
#endif
