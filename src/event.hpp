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
            };

            enum class State {
                Pressed,
                Released,
            };

            enum class Click {
                Single,
                Double,
            };

            enum class Button {
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

            MouseEvent(SDL_MouseButtonEvent event) {
                // TODO this will need clean up, there are more types to account for
                this->type = event.type == SDL_MOUSEBUTTONDOWN ? Type::Down : Type::Up;
                this->state = event.state == SDL_PRESSED ? State::Pressed : State::Released;
                this->click = event.clicks == 1 ? Click::Single : Click::Double;
                this->button = event.button == SDL_BUTTON_LEFT ? Button::Left : Button::Right;
                this->x = event.x;
                this->y = event.y;
            }
    };
#endif