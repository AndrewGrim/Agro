#include "timer.hpp"

u32 timerCallbackWrapper(u32 interval, void *real_callback) {
    return (*static_cast<std::function<u32(u32 interval)>*>(real_callback))(interval);
}

Timer::Timer() {}

Timer::Timer(u32 after, std::function<u32(u32 interval)> callback) {
    real_callback = std::make_shared<std::function<u32(u32 interval)>>(callback);
    id = SDL_AddTimer(after, timerCallbackWrapper, (void*)(real_callback.get()));
}
