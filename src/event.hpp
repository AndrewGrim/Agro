#ifndef EVENT_HPP
    #define EVENT_HPP

    #include <SDL.h>

    // TODO rework event
    // base event class?? im not sure what the benefits would be yet
    // but definitely split mousebutton and mousemotion events to
    // avoid all this extra state thats really not relevant
    // motion pressed state is covered by app state anyway
    // also add timestamp
    struct MouseEvent {
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
            this->button = this->handleButton(event.button);
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

        Button handleButton(int button) {
            switch (button) {
                case SDL_BUTTON_LEFT: return Button::Left;
                case SDL_BUTTON_MIDDLE: return Button::Middle;
                case SDL_BUTTON_RIGHT: return Button::Right;
                case SDL_BUTTON_X1: return Button::X1;
                case SDL_BUTTON_X2: return Button::X2;
            }
            return Button::None;
        }
    };

    struct ScrollEvent {
        Uint32 timestamp;
        int x;
        int y;

        ScrollEvent(SDL_MouseWheelEvent event) {
            this->timestamp = event.timestamp;
            this->x = event.x;
            this->y = event.y;
            if (event.direction) {
                this->x *= -1;
                this->y *= -1;
            }
        }
    };
#endif
