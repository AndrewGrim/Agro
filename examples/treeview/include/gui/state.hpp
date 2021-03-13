#ifndef STATE_HPP
    #define STATE_HPP
    class State {
        public:
            void *focused;
            void *hovered;
            void *pressed;
            void *tooltip;
        
            State() {
                this->focused = nullptr;
                this->hovered = nullptr;
                this->pressed = nullptr;
                this->tooltip = nullptr;
            }
    };
#endif