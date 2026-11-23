#include "chrono.h"
#include <inttypes.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
static uint64_t get_time_us(void)
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
static uint64_t get_time_us(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
    return (uint64_t)ts.tv_sec * UINT64_C(1000000) + (uint64_t)ts.tv_nsec / 1000;
}
#endif

static volatile uint32_t sink;
static void measured_operation(void)
{
    uint32_t i;
    for (i = 0; i < 1000000u; ++i) sink += i;
}

int main(void)
{
    chrono_t timer;
    uint64_t elapsed;
    if (chrono_init(&timer, get_time_us) != 0) return 1;
    if (chrono_start(&timer) != 0) return 1;
    measured_operation();
    if (chrono_elapsed_us(&timer, &elapsed) != 0) return 1;
    printf("Elapsed: %" PRIu64 " us\n", elapsed);
    return 0;
}
