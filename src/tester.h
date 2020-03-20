#ifndef TESTER_HEADER_H
#define TESTER_HEADER_H
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define assert_int(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%d")
#define assert_uint(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%ud")
#define assert_longint(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%ld")
#define assert_longuint(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%lud")
#define assert_float(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%f")
#define assert_double(expected, cmp, actual) ASSERT_BASIC(expected, cmp, actual, "%lf")
#define ASSERT_BASIC(expected, cmp, actual, type) ASSERT( \
    expected,                                             \
    actual,                                               \
    expected cmp actual,                                  \
    "expected \"" type "\" is not \"" #cmp "\" actual \"" type "\"")
#define ASSERT(expected, actual, comparison, message)    \
    if (!(comparison))                                   \
    {                                                    \
        fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);  \
        fprintf(stderr, message "\n", expected, actual); \
        fflush(stderr);                                  \
        exit(1);                                         \
    }

#define BEGIN_TEST(name)                                                    \
    static int test_##name(void)                                            \
    {                                                                       \
        double *timing = mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, \
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);           \
        char __name[] = #name;                                              \
        pid_t pid = fork();                                                 \
        if (pid < 0)                                                        \
            return -1;                                                      \
        if (pid == 0)                                                       \
        {                                                                   \
            clock_t __timer = clock();

// These need to be separate to keep line numbers
#define END_TEST                                                      \
    __timer = clock() - __timer;                                      \
    *timing = ((double)__timer) / CLOCKS_PER_SEC;                     \
    exit(0);                                                          \
    }                                                                 \
    else                                                              \
    {                                                                 \
        int status;                                                   \
        waitpid(pid, &status, 0);                                     \
        status = WIFEXITED(status) ? WEXITSTATUS(status) : -1;        \
        if (status == 0)                                              \
        {                                                             \
            printf("%s: \"%s\" \033[1;92mPASS ✔️\033[0m (%fs)\n", \
                   __FILE__, __name, *timing);                        \
        }                                                             \
        else if (status < 0)                                          \
        {                                                             \
            printf("%s: \"%s\" \033[1;93mUNSURE ?\033[0m\n",          \
                   __FILE__, __name);                                 \
        }                                                             \
        else                                                          \
        {                                                             \
            printf("%s: \"%s\" \033[1;91mFAIL ❌\033[0m\n",            \
                   __FILE__, __name);                                 \
        }                                                             \
        munmap(timing, sizeof(double));                               \
        fflush(stdout);                                               \
        return status;                                                \
    }                                                                 \
    }

#define RUN_TEST(name) test_##name()
#endif
