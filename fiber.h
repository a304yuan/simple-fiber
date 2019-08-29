#ifndef FIBER_H
#define FIBER_H

#include <time.h>

#define _YIELD_POINT_LABEL(line) YIELD_POINT##line
#define YIELD_POINT_LABEL(line) _YIELD_POINT_LABEL(line)

typedef struct fiber fiber;
typedef int (*fiber_start_func)(fiber * fb, void * arg);
typedef void (*fiber_deallocator)(fiber * fb);

enum fiber_status {
    FIBER_RUNNING,
    FIBER_PAUSED,
    FIBER_EXITED
};

struct fiber {
    enum fiber_status status;
    clock_t next_run;
    fiber_start_func func;
    fiber_deallocator defunc;
    void * arg;
    void * yield_point;
};

extern void fiber_init(int threads, long max_fibers);
extern int fiber_create(fiber_start_func func, void * arg, fiber_deallocator defunc);

// macro functions
#define fiber_start(fb) \
    do { \
        if (fb->yield_point) { \
            goto *(fb->yield_point); \
        } \
    } while (0)

#define fiber_yield(fb) \
    do { \
        fb->yield_point = &&YIELD_POINT_LABEL(__LINE__); \
        return FIBER_PAUSED; \
        YIELD_POINT_LABEL(__LINE__): break; \
    } while (0)

#define fiber_sleep(fb, msecs) \
    do { \
        fb->next_run = clock() + CLOCKS_PER_SEC / 1000 * msecs; \
        fiber_yield(fb); \
    } while (0)

#define fiber_exit(fb) return FIBER_EXITED

#endif /* end of include guard FIBER_H */
