# Command-line interface

Version: **{{ chrono_version }}**

The native `chrono` executable measures elapsed wall-clock time across
invocations. It is built on Windows and POSIX targets when
`CHRONO_BUILD_CLI=ON` and cross-compilation is not enabled.

## Usage

```text
chrono [OPTION]
```

| Option | Result |
| --- | --- |
| `-h`, `--help` | Print help and exit successfully |
| `--reset` | Delete the saved start time |
| none | Print elapsed microseconds and save an initial timestamp if needed |

With no option, the first run saves the current wall-clock time and prints `0`.
Later runs read that value and print elapsed microseconds.

```text
chrono
0
chrono
1520341
chrono --reset
Start time reset.
chrono
0
```

Run `chrono --help` for the built-in usage text.

## State file

The start timestamp is stored in:

- Windows: `%APPDATA%\chrono-start-us`
- Unix: `$HOME/.chrono-start-us`

The file contains a decimal microsecond timestamp and a newline.

The CLI uses the system real-time clock rather than a monotonic clock so that
elapsed time remains meaningful across process and system restarts. Clock
adjustments can affect the result. If the system clock moves before the saved
timestamp, `chrono` prints an error and exits with a nonzero status.

Delete the state file or run `chrono --reset` to begin a new measurement.
