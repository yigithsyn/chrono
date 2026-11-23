# chrono-c

Minimal C99 microsecond stopwatch library for embedded systems.

## Layout

- `include/chrono.h`: public API
- `src/chrono.c`: platform-independent implementation
- `examples/main.c`: Windows/Linux clock adapter and usage
- `tests/test_chrono.c`: deterministic tests

## Build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `build/chrono_example` on Linux or `build/Debug/chrono_example.exe` with multi-config Windows generators (exact path depends on generator).

For embedded cross-compilation, supply your own `uint64_t get_time_us(void)` using a monotonic hardware timer or SDK clock, then call `chrono_init(&timer, get_time_us)`. Disable host examples/tests:

```sh
cmake -S . -B build-embedded -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake -DCHRONO_BUILD_EXAMPLES=OFF -DCHRONO_BUILD_TESTS=OFF
```

The supplied clock must represent elapsed microseconds and should be monotonic modulo 2^64. The library measures elapsed wall-clock time, including interrupts or preemption, not exclusive CPU execution time. Resolution and accuracy depend on the supplied clock. Call `chrono_start` before `chrono_elapsed_us`.
