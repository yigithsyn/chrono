# C API

Include the public header:

```c
#include "chrono.h"
```

## Clock function

```c
typedef uint64_t (*chrono_clock_fn)(void);
uint64_t chrono_get_time_us(void);
```

`chrono_clock_fn` is a function that returns elapsed microseconds. Supplied
clocks should be monotonic modulo `UINT64_MAX + 1`. `chrono_get_time_us()` is
the built-in clock on Windows and POSIX host builds.

The Windows implementation uses the high-resolution performance counter. The
POSIX implementation uses `CLOCK_MONOTONIC`. Clock resolution and accuracy depend
on the platform.

## Timer functions

```c
typedef struct {
    chrono_clock_fn clock;
    uint64_t start_us;
} chrono_t;

int chrono_init(chrono_t *timer, chrono_clock_fn clock);
int chrono_start(chrono_t *timer);
int chrono_elapsed_us(const chrono_t *timer, uint64_t *elapsed_us);
```

`chrono_init()` assigns the clock and resets the timer. It returns `0` on success
and `-1` if `timer` or `clock` is `NULL`.

`chrono_start()` records the clock value in `timer->start_us`. It returns `0`
on success and `-1` for a `NULL` timer or an uninitialized clock.

`chrono_elapsed_us()` reads the clock again and writes unsigned modular
subtraction from `timer->start_us`. It returns `0` on success and `-1` for
invalid arguments.

The API does not allocate memory and has no thread synchronization requirement
for a timer instance. Do not call operations on the same timer concurrently.

## Example

```c
#include "chrono.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    chrono_t timer;
    uint64_t elapsed;

    if (chrono_init(&timer, chrono_get_time_us) != 0)
        return 1;
    if (chrono_start(&timer) != 0)
        return 1;

    /* Perform work here. */

    if (chrono_elapsed_us(&timer, &elapsed) != 0)
        return 1;
    printf("Elapsed: %" PRIu64 " us\n", elapsed);
    return 0;
}
```
