#ifndef FIBER_H
#define FIBER_H

#include <stdlib.h>
#include <string.h>
#include <threads.h>

typedef struct fiber fiber;
typedef enum fiber_status fiber_status;
typedef int (*fiber_start_func)(fiber * fb, void * arg);

enum fiber_status {
    FIBER_RUNNING = 2,
    FIBER_PAUSE = 3,
    FIBER_EXITED = 4
};

struct fiber {
    fiber_start_func func;
    void * arg;
    size_t frame_size;
    fiber_status status;
    fiber * next;
    void * frame;
};

extern void fiber_init(int threads);
extern fiber * fiber_create(fiber_start_func func, void * arg);

// macro functions
#define fiber_start(fb) \
    do { \
        if (fb->frame) { \
            void * _bp, * _sp; \
            __asm__( \
                "mov %%rbp, %0\n\t" \
                "mov %%rsp, %1\n\t" \
                : "=r"(_bp), "=r"(_sp) \
            ); \
            memcpy(_bp - fb->frame_size, fb->frame, fb->frame_size); \
            goto YIELD_POINT; \
        } \
    } while (0)

#define fiber_yield(fb) \
    do { \
        void * _bp, * _sp; \
        __asm__( \
            "mov %%rbp, %0\n\t" \
            "mov %%rsp, %1\n\t" \
            : "=r"(_bp), "=r"(_sp) \
        ); \
        size_t frame_size = _bp - _sp; \
        if (fb->frame_size < frame_size) { \
            if (fb->frame) { \
                fb->frame = realloc(fb->frame, frame_size); \
            } \
            else { \
                fb->frame = malloc(frame_size); \
            } \
        } \
        memcpy(fb->frame, _sp, frame_size); \
        fb->frame_size = frame_size; \
        return 0; \
        YIELD_POINT: break; \
    } while (0)

#define fiber_exit(fb) \
    do { \
        fb->status = FIBER_EXITED; \
        return FIBER_EXITED; \
    } while (0)

inline fiber_status fiber_check_status(fiber * fb) {
    return fb->status;
}

extern void fiber_join(fiber * fb);
extern void fiber_detach(fiber *fb);

#endif /* end of include guard FIBER_H */
