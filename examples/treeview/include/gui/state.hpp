#ifndef STATE_HPP
    #define STATE_HPP
    class State {
        public:
            void *focused;
            void *hovered;
            void *pressed;
        
            State() {
                this->focused = nullptr;
                this->hovered = nullptr;
                this->pressed = nullptr;
            }
    };
#endif