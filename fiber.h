#ifndef FIBER_H
#define FIBER_H

#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

typedef int (*fiber_start_func)(fiber * fb, void * arg);
typedef struct fiber fiber;
struct fiber {
    jmp_buf jbuf;
    size_t frame_size;
    struct {
        void * bp;
        void * sp;
    } registers;
    enum {
        FIBER_RUNNING;
        FIBER_PAUSE;
        FIBER_EXITED;
    } status;
    fiber * next;
    void * frame;
};

void fiber_init(int threads);
fiber * fiber_create(fiber_start_func fun, void * arg);
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
    fb->frame_size = _bp - _sp;
    
}
void fiber_exit();

void fiber_join(fiber * fb);
void fiber_detach(fiber *fb);

#endif /* end of include guard FIBER_H */
