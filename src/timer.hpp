#ifndef TIMER_HPP
    #define TIMER_HPP

    #include <SDL.h>

    struct Timer {
        SDL_TimerID id = -1;
        uint64_t index = 0;

        Timer(uint32_t after, uint32_t(*callback)(uint32_t, void*), void *data) {
            id = SDL_AddTimer(after, callback, data);
        }
    };
#endif
