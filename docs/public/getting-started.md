# Build and install

## Requirements

chrono requires CMake 3.16 or newer and a C99 compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

On Visual Studio or another multi-config generator, the example and test
executables are usually under a configuration subdirectory such as `Debug`.

## CMake options

| Option                  | Default | Meaning                                      |
| ----------------------- | ------- | -------------------------------------------- |
| `CHRONO_BUILD_EXAMPLES` | `ON`    | Build the example executable                 |
| `CHRONO_BUILD_TESTS`    | `ON`    | Build and register the CTest test            |
| `CHRONO_BUILD_CLI`      | `ON`    | Build the native persistent elapsed-time CLI |

## Installation

```sh
cmake -S . -B build
cmake --build build
cmake --install build
```

The default installation prefix is `%LOCALAPPDATA%\Programs\chrono` on Windows
and `$HOME/.local` on Unix. Override it with `-DCMAKE_INSTALL_PREFIX=<directory>`.

Installation includes `libchrono.a` or the platform equivalent, `chrono.h`, the
optional `chrono` executable and the installed CMake package configuration.
