#ifndef TIMER_HPP
    #define TIMER_HPP

    #include <functional>
    #include <SDL.h>

    uint32_t timerCallbackWrapper(uint32_t interval, void *real_callback);

    struct Timer {
        SDL_TimerID id = -1;
        uint64_t index = 0;
        std::shared_ptr<std::function<uint32_t(uint32_t interval)>> real_callback;

        Timer(uint32_t after, std::function<uint32_t(uint32_t interval)> callback);
    };
#endif
