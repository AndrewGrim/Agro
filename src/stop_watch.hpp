#ifndef STOP_WATCH_HPP
    #define STOP_WATCH_HPP

    #include <time.h>

    #include "common/number_types.h"

    // TODO apparently on POSIX CLOCKS_PER_SEC is meant to be 1'000'000 but 1'000 on windows...
    // we could try to use the performance counter on windows instead
    struct StopWatch {
        clock_t start_time = 0;
        clock_t stop_time = 0;
        clock_t duration = 0;

        StopWatch() {}

        StopWatch start() {
            start_time = clock();
            return *this;
        }

        StopWatch stop() {
            stop_time = clock();
            duration = stop_time - start_time;
            return *this;
        }

        u64 asSeconds() {
            return duration / CLOCKS_PER_SEC;
        }

        u64 asMilliseconds() {
            return duration / (CLOCKS_PER_SEC / 1'000);
        }

        u64 asMicroseconds() {
            return duration / (CLOCKS_PER_SEC / 1'000'000);
        }
    };
#endif
