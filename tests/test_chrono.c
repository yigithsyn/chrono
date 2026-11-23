#include "chrono.h"
#include <assert.h>
#include <stdint.h>

static uint64_t fake_time;
static uint64_t fake_clock(void) { return fake_time; }

int main(void)
{
    chrono_t timer;
    uint64_t elapsed = 0;
    assert(chrono_init(0, fake_clock) == -1);
    assert(chrono_init(&timer, 0) == -1);
    assert(chrono_init(&timer, fake_clock) == 0);
    fake_time = 100;
    assert(chrono_start(&timer) == 0);
    fake_time = 125;
    assert(chrono_elapsed_us(&timer, &elapsed) == 0);
    assert(elapsed == 25);
    assert(chrono_elapsed_us(&timer, 0) == -1);
    fake_time = UINT64_MAX - 3;
    assert(chrono_start(&timer) == 0);
    fake_time = 2;
    assert(chrono_elapsed_us(&timer, &elapsed) == 0);
    assert(elapsed == 6);
    return 0;
}
