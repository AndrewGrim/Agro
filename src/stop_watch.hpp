#ifndef STOP_WATCH_HPP
    #define STOP_WATCH_HPP

    #include "common/number_types.h"

    #ifdef __WIN32
        #include <profileapi.h>

        struct StopWatch {
            LARGE_INTEGER start_time;
            LARGE_INTEGER stop_time;
            LARGE_INTEGER duration;
            LARGE_INTEGER frequency;

            StopWatch() {
                QueryPerformanceFrequency(&frequency);
            }

            StopWatch start() {
                QueryPerformanceCounter(&start_time);
                return *this;
            }

            StopWatch stop() {
                QueryPerformanceCounter(&stop_time);
                duration.QuadPart = stop_time.QuadPart - start_time.QuadPart;
                return *this;
            }

            u64 asSeconds() {
                return duration.QuadPart / frequency.QuadPart;
            }

            u64 asMilliseconds() {
                return duration.QuadPart * 1'000 / frequency.QuadPart;
            }

            u64 asMicroseconds() {
                return duration.QuadPart * 1'000'000 / frequency.QuadPart;
            }
        };
    #else
        #include <time.h>

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
#endif
