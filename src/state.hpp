#ifndef STATE_HPP
    #define STATE_HPP

    class Widget;

    struct State {
        Widget *soft_focused = nullptr;
        Widget *hard_focused = nullptr;
        Widget *hovered = nullptr;
        Widget *pressed = nullptr;
        Widget *tooltip = nullptr;
    };
#endif
