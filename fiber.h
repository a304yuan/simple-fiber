#ifndef FIBER_H
#define FIBER_H

#include <stdlib.h>
#include <string.h>
#include <threads.h>

typedef int (*fiber_start_func)(fiber * fb, void * arg);
typedef struct fiber fiber;
typedef enum fiber_status fiber_status;

enum fiber_status {
    FIBER_RUNNING = 2;
    FIBER_PAUSE = 3;
    FIBER_EXITED = 4;
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
extern fiber * fiber_create(fiber_start_func fun, void * arg);
// macro functions
void fiber_start() {
    do {
        if (fb->frame) {
            // restore stack frame
            void * _bp, * _sp;
            __asm__(
                "mov %%rbp, %0\n\t"
                "mov %%rsp, %1\n\t"
                : "=r"(_bp), "=r"(_sp)
            );
            memcpy(_bp - fb->frame_size, fb->frame, fb->frame_size);
            goto YIELD_POINT;
        }
    } while(0);
}

void fiber_yield() {
    do {
        void * _bp, * _sp;
        __asm__(
            "mov %%rbp, %0\n\t"
            "mov %%rsp, %1\n\t"
            : "=r"(_bp), "=r"(_sp)
        );
        size_t frame_size = _bp - _sp;
        if (fb->frame_size < frame_size) {
            if (fb->frame) {
                fb->frame = realloc(fb->frame, frame_size);
            }
            else {
                fb->frame = malloc(frame_size);
            }
        }
        // copy stack frame
        memcpy(fb->frame, _sp, frame_size);
        fb->frame_size = frame_size;
        // back to thread main loop
        return 0;
        YIELD_POINT: break;
    } while(0);
}

void fiber_exit() {
    fb->status = FIBER_EXITED;
    return FIBER_EXITED;
}

fiber_status fiber_check_status(fiber * fb) {
    return fb->status;
}

void fiber_join(fiber * fb);
void fiber_detach(fiber *fb);

#endif /* end of include guard FIBER_H */
