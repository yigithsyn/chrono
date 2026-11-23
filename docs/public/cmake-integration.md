# CMake integration

## Installed package

After installing chrono:

```cmake
find_package(chrono CONFIG REQUIRED)
target_link_libraries(my_application PRIVATE chrono::chrono)
```

Configure the application with `-DCMAKE_PREFIX_PATH=<chrono-install-prefix>`
if the installation prefix is not searched automatically.

The exported target carries the include directory and C99 compile requirement.

## Source integration

```cmake
add_subdirectory(chrono)
target_link_libraries(my_application PRIVATE chrono::chrono)
```

Set `CHRONO_BUILD_EXAMPLES=OFF`, `CHRONO_BUILD_TESTS=OFF` and
`CHRONO_BUILD_CLI=OFF` before adding the directory if those targets are not
needed.

## Embedded integration

For a cross-compilation build, supply the target clock and disable the native
CLI and host-only targets:

```sh
cmake -S . -B build-embedded \
  -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake \
  -DCHRONO_BUILD_CLI=OFF \
  -DCHRONO_BUILD_EXAMPLES=OFF \
  -DCHRONO_BUILD_TESTS=OFF
cmake --build build-embedded
```
