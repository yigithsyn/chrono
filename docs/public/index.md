# chrono-c

Version: **{{ chrono_version }}**

chrono-c is a minimal C99 stopwatch library for embedded and native systems.
It provides a small API for initializing a timer, supplying a microsecond clock,
starting a measurement and reading the elapsed time.

Native Windows and POSIX builds include built-in monotonic clocks. Embedded
applications can supply their own hardware or SDK timer function.

The project also provides a persistent elapsed-time command-line tool for
Windows and POSIX. It saves a wall-clock start time and reports elapsed
microseconds across runs and system restarts.

See [build and install](getting-started.md), [the C API](api.md),
[the command-line interface](cli.md), [CMake integration](cmake-integration.md),
and [embedded use](embedded.md).
