#include "chrono.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
typedef wchar_t path_char;
#define PATH_LENGTH wcslen
#define PATH_COPY wcscpy
#define PATH_APPEND wcscat
#define OPEN_STATE(path) _wfopen((path), L"rb")
static const path_char state_suffix[] = L"\\chrono-start-us";
#else
#include <time.h>
#include <unistd.h>
typedef char path_char;
#define PATH_LENGTH strlen
#define PATH_COPY strcpy
#define PATH_APPEND strcat
#define OPEN_STATE(path) fopen((path), "rb")
static const path_char state_suffix[] = "/.chrono-start-us";
#endif

static uint64_t realtime_us(void)
{
#ifdef _WIN32
    FILETIME time;
    ULARGE_INTEGER ticks;
    GetSystemTimeAsFileTime(&time);
    ticks.LowPart = time.dwLowDateTime;
    ticks.HighPart = time.dwHighDateTime;
    const uint64_t epoch = UINT64_C(116444736000000000);
    return ticks.QuadPart > epoch ? (ticks.QuadPart - epoch) / 10 : 0;
#else
    struct timespec time;
    if (clock_gettime(CLOCK_REALTIME, &time) != 0 || time.tv_sec < 0)
        return 0;
    return (uint64_t)time.tv_sec * UINT64_C(1000000) + (uint64_t)time.tv_nsec / 1000;
#endif
}

/* Return 1 for a missing file, 0 for a valid timestamp, and -1 for an error. */
static int read_start(const path_char *path, uint64_t *start)
{
    FILE *file = OPEN_STATE(path);
    if (!file)
        return errno == ENOENT ? 1 : -1;

    char value[22];
    size_t size = fread(value, 1, sizeof(value), file);
    int failed = ferror(file);
    if (fclose(file) != 0 || failed || size < 2 ||
        size == sizeof(value) || value[size - 1] != '\n')
        return -1;

    value[size - 1] = '\0';
    if (strspn(value, "0123456789") != size - 1)
        return -1;
    errno = 0;
    uintmax_t parsed = strtoumax(value, NULL, 10);
    if (errno == ERANGE || parsed == 0 || parsed > UINT64_MAX)
        return -1;

    *start = (uint64_t)parsed;
    return 0;
}

static int write_start(FILE *file, uint64_t start)
{
    if (!file)
        return -1;
    int failed = fprintf(file, "%" PRIu64 "\n", start) < 0;
    return fclose(file) == 0 && !failed ? 0 : -1;
}

/* Publish the completed file only if another invocation has not created it. */
static int save_start(const path_char *directory, const path_char *path, uint64_t start)
{
#ifdef _WIN32
    wchar_t temporary[MAX_PATH];
    if (!GetTempFileNameW(directory, L"chr", 0, temporary))
        return -1;
    int result = write_start(_wfopen(temporary, L"wb"), start);
    if (result == 0) {
        if (MoveFileW(temporary, path))
            return 0;
        DWORD error = GetLastError();
        result = error == ERROR_ALREADY_EXISTS || error == ERROR_FILE_EXISTS ? 1 : -1;
    }
    DeleteFileW(temporary);
    return result;
#else
    (void)directory;
    const char suffix[] = ".XXXXXX";
    char *temporary = malloc(strlen(path) + sizeof(suffix));
    if (!temporary)
        return -1;
    strcpy(temporary, path);
    strcat(temporary, suffix);
    int fd = mkstemp(temporary);
    if (fd < 0) {
        free(temporary);
        return -1;
    }
    FILE *file = fdopen(fd, "wb");
    if (!file)
        close(fd);
    int result = write_start(file, start);
    if (result == 0 && link(temporary, path) != 0)
        result = errno == EEXIST ? 1 : -1;
    unlink(temporary);
    free(temporary);
    return result;
#endif
}

static int run(const path_char *directory, const path_char *path)
{
    chrono_t timer;
    chrono_init(&timer, realtime_us);
    int result = read_start(path, &timer.start_us);
    if (result == 1) {
        chrono_start(&timer);
        if (timer.start_us == 0) {
            fputs("chrono: could not read the system clock\n", stderr);
            return EXIT_FAILURE;
        }
        result = save_start(directory, path, timer.start_us);
        if (result == 0) {
            puts("0");
            return EXIT_SUCCESS;
        }
        if (result < 0) {
            fputs("chrono: could not save the start time\n", stderr);
            return EXIT_FAILURE;
        }
        result = read_start(path, &timer.start_us);
    }
    if (result != 0) {
        fputs("chrono: could not read a valid saved start time\n", stderr);
        return EXIT_FAILURE;
    }

    uint64_t elapsed;
    chrono_elapsed_us(&timer, &elapsed);
    /* Unsigned subtraction wraps if the wall clock goes backward. */
    if (elapsed > UINT64_MAX - timer.start_us) {
        fputs("chrono: system clock unavailable or earlier than the saved start time\n", stderr);
        return EXIT_FAILURE;
    }

    printf("%" PRIu64 "\n", elapsed);
    return EXIT_SUCCESS;
}

static void print_help(const char *program)
{
    printf("Usage: %s [OPTION]\n\n", program);
    puts("Print the elapsed wall-clock time in microseconds since the first run.");
    puts("");
    puts("Options:");
    puts("  -h, --help  Show this help text and exit.");
    puts("  --reset     Delete the saved start time; the next run starts a new measurement.");
    puts("");
    puts("With no option, run the stopwatch and print elapsed microseconds.");
}

static int reset_start(const path_char *path)
{
#ifdef _WIN32
    if (DeleteFileW(path))
        return 0;
    DWORD error = GetLastError();
    return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ? 0 : -1;
#else
    if (remove(path) == 0)
        return 0;
    return errno == ENOENT ? 0 : -1;
#endif
}

int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(stderr, "chrono: too many arguments\n");
        fputs("Try 'chrono --help' for more information.\n", stderr);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "--reset") != 0) {
            fprintf(stderr, "chrono: unknown option '%s'\n", argv[1]);
            fputs("Try 'chrono --help' for more information.\n", stderr);
            return EXIT_FAILURE;
        }
    }

#ifdef _WIN32
    const path_char *directory = _wgetenv(L"APPDATA");
    const char *variable = "APPDATA";
#else
    const path_char *directory = getenv("HOME");
    const char *variable = "HOME";
#endif
    if (!directory || !*directory) {
        fprintf(stderr, "chrono: %s is not set\n", variable);
        return EXIT_FAILURE;
    }

    path_char *path = malloc(PATH_LENGTH(directory) * sizeof(*path) + sizeof(state_suffix));
    if (!path) {
        fputs("chrono: could not allocate the state path\n", stderr);
        return EXIT_FAILURE;
    }
    PATH_COPY(path, directory);
    PATH_APPEND(path, state_suffix);
    int result;
    if (argc == 2)
        result = reset_start(path);
    else
        result = run(directory, path);
    if (argc == 2 && result == 0)
        puts("Start time reset.");
    else if (argc == 2)
        fputs("chrono: could not reset the start time\n", stderr);
    free(path);
    return result;
}
