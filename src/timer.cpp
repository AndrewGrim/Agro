#include "timer.hpp"

uint32_t timerCallbackWrapper(uint32_t interval, void *real_callback) {
    return (*static_cast<std::function<uint32_t(uint32_t interval)>*>(real_callback))(interval);
}

Timer::Timer(uint32_t after, std::function<uint32_t(uint32_t interval)> callback) {
    real_callback = std::make_shared<std::function<uint32_t(uint32_t interval)>>(callback);
    id = SDL_AddTimer(after, timerCallbackWrapper, (void*)(real_callback.get()));
}
