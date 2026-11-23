#ifndef CHRONO_H
#define CHRONO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Monotonic microsecond clock supplied by the target platform. */
typedef uint64_t (*chrono_clock_fn)(void);

typedef struct {
    chrono_clock_fn clock;
    uint64_t start_us;
} chrono_t;

/* Returns 0 on success, -1 for invalid arguments. */
int chrono_init(chrono_t *timer, chrono_clock_fn clock);
int chrono_start(chrono_t *timer);
/* Returns 0 on success, -1 for invalid arguments. */
int chrono_elapsed_us(const chrono_t *timer, uint64_t *elapsed_us);

#ifdef __cplusplus
}
#endif

#endif
