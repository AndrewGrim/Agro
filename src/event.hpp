#ifndef EVENT_HPP
    #define EVENT_HPP

    #include <SDL2/SDL.h>

    class Event {
        public:
            enum class Type {
                MouseDown,
            };

            Event(Type type) {
                this->m_type = type;
            }

            Type type() {
                return this->m_type;
            }

        private:
            Type m_type;
    };

    class MouseEvent {
        public:
            enum class Type {
                Down,
                Up,
                Motion,
            };

            enum class State {
                Pressed,
                Released,
            };

            enum class Click {
                None,
                Single,
                Double,
            };

            enum class Button {
                None,
                Left,
                Middle,
                Right,
                X1,
                X2,
            };

            Type type;
            State state;
            Click click;
            Button button;
            int x;
            int y;
            int xrel;
            int yrel;

            MouseEvent(SDL_MouseButtonEvent event) {
                this->type = event.type == SDL_MOUSEBUTTONDOWN ? Type::Down : Type::Up;
                this->state = event.state == SDL_PRESSED ? State::Pressed : State::Released;
                this->click = event.clicks == 1 ? Click::Single : Click::Double; // Note: this only cover single and double click
                this->button = this->handle_button(event.button);
                this->x = event.x;
                this->y = event.y;
                this->xrel = 0;
                this->yrel = 0;
            }

            MouseEvent(SDL_MouseMotionEvent event) {
                this->type = Type::Motion;
                this->state = event.state == SDL_PRESSED ? State::Pressed : State::Released;
                this->click = Click::None;
                this->button = Button::None;
                this->x = event.x;
                this->y = event.y;
                this->xrel = event.xrel;
                this->yrel = event.yrel;
            }

            Button handle_button(int button) {
                switch (button) {
                    case SDL_BUTTON_LEFT:
                        return Button::Left;
                    case SDL_BUTTON_MIDDLE:
                        return Button::Middle;
                    case SDL_BUTTON_RIGHT:
                        return Button::Right;
                    case SDL_BUTTON_X1:
                        return Button::X1;
                    case SDL_BUTTON_X2:
                        return Button::X2;
                }

                return Button::None;
            }
    };
#endif