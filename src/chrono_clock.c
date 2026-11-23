#include "chrono.h"

#ifdef _WIN32
#include <windows.h>
uint64_t chrono_get_time_us(void)
{
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    uint64_t whole = (uint64_t)(counter.QuadPart / frequency.QuadPart);
    uint64_t remainder = (uint64_t)(counter.QuadPart % frequency.QuadPart);
    return whole * UINT64_C(1000000) + remainder * UINT64_C(1000000) / (uint64_t)frequency.QuadPart;
}
#else
#include <time.h>
uint64_t chrono_get_time_us(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
    return (uint64_t)ts.tv_sec * UINT64_C(1000000) + (uint64_t)ts.tv_nsec / 1000;
}
#endif
