#ifndef TIMER_HPP
    #define TIMER_HPP

    #include <functional>
    #include <memory>

    #include <SDL.h>

    #include "common/number_types.h"

    u32 timerCallbackWrapper(u32 interval, void *real_callback);

    struct Timer {
        SDL_TimerID id = -1;
        std::shared_ptr<std::function<u32(u32 interval)>> real_callback;

        Timer();
        Timer(u32 after, std::function<u32(u32 interval)> callback);
    };
#endif
