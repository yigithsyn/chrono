# Embedded use

chrono-c can be used without host timing functions by supplying a clock for
the target hardware.

```c
#include "chrono.h"

extern uint64_t target_get_time_us(void);

void measure(void)
{
    chrono_t timer;
    uint64_t elapsed;

    if (chrono_init(&timer, target_get_time_us) != 0)
        return;
    if (chrono_start(&timer) != 0)
        return;

    /* Run the operation under measurement. */

    if (chrono_elapsed_us(&timer, &elapsed) == 0) {
        /* Use elapsed microseconds. */
    }
}
```

## Clock requirements

The supplied clock should:

- return elapsed microseconds;
- advance monotonically modulo `2^64`;
- keep an appropriate resolution for the application;
- continue advancing while the target is interrupted or preempted.

Elapsed wall-clock time includes interrupts and preemption. It is not equivalent
to exclusive CPU execution time. The clock wrap behavior is handled by the
library's unsigned 64-bit subtraction.

Configure an embedded build with examples and tests disabled:

```sh
cmake -S . -B build-embedded \
  -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake \
  -DCHRONO_BUILD_CLI=OFF \
  -DCHRONO_BUILD_EXAMPLES=OFF \
  -DCHRONO_BUILD_TESTS=OFF
cmake --build build-embedded
```
