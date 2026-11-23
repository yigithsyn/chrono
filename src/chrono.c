#include "chrono.h"

int chrono_init(chrono_t *timer, chrono_clock_fn clock)
{
    if (timer == 0 || clock == 0) return -1;
    timer->clock = clock;
    timer->start_us = 0;
    return 0;
}

int chrono_start(chrono_t *timer)
{
    if (timer == 0 || timer->clock == 0) return -1;
    timer->start_us = timer->clock();
    return 0;
}

int chrono_elapsed_us(const chrono_t *timer, uint64_t *elapsed_us)
{
    if (timer == 0 || timer->clock == 0 || elapsed_us == 0) return -1;
    *elapsed_us = timer->clock() - timer->start_us;
    return 0;
}
