#include "chrono.h"
#include <inttypes.h>
#include <stdio.h>

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
    if (chrono_init(&timer, chrono_get_time_us) != 0) return 1;
    if (chrono_start(&timer) != 0) return 1;
    measured_operation();
    if (chrono_elapsed_us(&timer, &elapsed) != 0) return 1;
    printf("Elapsed: %" PRIu64 " us\n", elapsed);
    return 0;
}
