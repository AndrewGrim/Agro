#ifndef EVENT_HPP
    #define EVENT_HPP

    #include <SDL.h>

    #include "common/number_types.h"
    #include "core/string.hpp"

    enum {
        AGRO_PULSE = 0,
        AGRO_FONT_HEIGHT_CHANGED = 1
    };

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
        i32 x;
        i32 y;
        i32 xrel;
        i32 yrel;

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

        Button handleButton(i32 button) {
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
        u32 timestamp;
        i32 x;
        i32 y;

        ScrollEvent(SDL_MouseWheelEvent event) {
            this->timestamp = event.timestamp;
            this->x = event.x;
            this->y = event.y * -1;
            if (event.direction) {
                this->x *= -1;
                this->y *= -1;
            }
        }
    };

    enum class FocusEvent {
        Activate, // Active = STATE_HARD_FOCUSED
        Forward, // Tab, Forward = STATE_SOFT_FOCUSED
        Reverse // Shift+Tab, Reverse = STATE_SOFT_FOCUSED
    };

    class Widget;
    struct Drawable;

    struct DragEvent {
        enum State {
            Default,
            PotentialStart,
            Dragging,
        };

        State state;
        Widget *origin;
        Drawable *preview;
        String content;

        DragEvent() : state{State::Default}, origin{nullptr}, preview{nullptr}, content{} {}
        DragEvent(Widget *origin, Drawable *preview, String content) :
             state{State::PotentialStart}, origin{origin}, preview{preview}, content{content} {}
    };
#endif
