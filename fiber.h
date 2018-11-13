#ifndef FIBER_H
#define FIBER_H

#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <threads.h>

typedef int (*fiber_start_func)(fiber * fb, jmp_buf * buf, void * arg);
typedef struct fiber fiber;
typedef struct thread thread;
typedef enum fiber_status fiber_status;

struct thread {
    jmp_buf buf;
};

enum fiber_status {
    FIBER_RUNNING = 1;
    FIBER_PAUSE = 2;
    FIBER_EXITED = 3;
};

struct fiber {
    jmp_buf buf;
    size_t frame_size;
    struct {
        void * bp;
        void * sp;
    } registers;
    fiber_status status;
    fiber * next;
    void * frame;
};

extern void fiber_init(int threads);
extern fiber * fiber_create(fiber_start_func fun, void * arg);
// macro functions
void fiber_yield() {
    void * _bp, * _sp;
    __asm__(
        "mov %%rbp, %0\n\t"
        "mov %%rsp, %1\n\t"
        : "=r"(_bp), "=r"(_sp)
    );
    fb->registers.bp = _bp;
    fb->registers.sp = _sp;
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
    fb->frame_size = _bp - _sp;
    // jump to thread main loop
    if (setjmp(fb->buf) == 0) {
        longjmp((*buf), FIBER_PAUSE);
    }
}

void fiber_exit() {
    longjmp((*buf), FIBER_EXITED);
}

fiber_status fiber_check_status(fiber * fb) {
    return fb->status;
}

void fiber_join(fiber * fb);
void fiber_detach(fiber *fb);

#endif /* end of include guard FIBER_H */
