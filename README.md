# chrono-c

Minimal C99 microsecond stopwatch library for embedded systems.

## Layout

- `include/chrono.h`: public API
- `src/chrono.c`: platform-independent implementation
- `src/chrono_clock.c`: Windows and POSIX monotonic clock
- `src/chrono_cli.c`: persistent elapsed-time CLI for Windows and POSIX
- `examples/main.c`: example using `chrono_init(&timer, chrono_get_time_us)`
- `tests/test_chrono.c`: deterministic tests

## Build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `build/chrono_example` on Linux or `build/Debug/chrono_example.exe` with multi-config Windows generators (exact path depends on generator).

## CLI

Run `build/chrono` on Linux or `build/Debug/chrono.exe` with a multi-config Windows generator.
The first run saves the current time and prints `0`. Each subsequent run prints the
elapsed time in microseconds since the first run, including across system restarts.

Use `chrono --help` or `chrono -h` to show the command usage and available options.
Use `chrono --reset` to delete the saved start time; the next run starts a new
measurement.

The state file is `%APPDATA%\chrono-start-us` on Windows and `$HOME/.chrono-start-us` on Unix.
Delete the state file to start over. The CLI uses the system wall clock, so clock
adjustments affect the reported duration. It returns an error if the system time
moves before the saved start time.

Native Windows and Unix builds include the CLI by default. Pass
`-DCHRONO_BUILD_CLI=OFF` to disable it. CMake installs the executable as `chrono`
in the installation prefix's binary directory.

## Embedded usage

For embedded cross-compilation, supply your own `uint64_t get_time_us(void)` using a monotonic hardware timer or SDK clock, then call `chrono_init(&timer, get_time_us)`. Disable host examples/tests:

```sh
cmake -S . -B build-embedded -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake -DCHRONO_BUILD_EXAMPLES=OFF -DCHRONO_BUILD_TESTS=OFF
```

The supplied clock must represent elapsed microseconds and should be monotonic modulo 2^64. The library measures elapsed wall-clock time, including interrupts or preemption, not exclusive CPU execution time. Resolution and accuracy depend on the supplied clock. Call `chrono_start` before `chrono_elapsed_us`.

## Documentation

```sh
python -m pip install -r docs/requirements.txt
python -m mkdocs build --strict
python -m mkdocs serve
```

The documentation is available as MkDocs content under `docs/public` and uses
the Read the Docs theme. It is configured for both local builds and Read the Docs
through `.readthedocs.yaml`.

## License

Licensed under the [MIT License](LICENSE).
