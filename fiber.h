#ifndef FIBER_H
#define FIBER_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdatomic.h>
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
    struct {
        uint64_t ax, bx, cx, dx, si, di;
    } registers;
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
            __asm__ __volatile__ ( \
                "mov %%rbp, %0\n\t" \
                "mov %%rsp, %1\n\t" \
                : "=r"(_bp), "=r"(_sp) \
            ); \
            atomic_thread_fence(memory_order_acq_rel); \
            memcpy(_sp, fb->frame, fb->frame_size); \
            atomic_thread_fence(memory_order_acq_rel); \
            __asm__ __volatile__ ( \
                "mov %0, %%rax\n\t" \
                "mov %1, %%rbx\n\t" \
                "mov %2, %%rcx\n\t" \
                "mov %3, %%rdx\n\t" \
                "mov %4, %%rsi\n\t" \
                "mov %5, %%rdi\n\t" \
                :: "r"(fb->registers.ax), "r"(fb->registers.bx), "r"(fb->registers.cx), "r"(fb->registers.dx), "r"(fb->registers.si), "r"(fb->registers.di) \
            ); \
            atomic_thread_fence(memory_order_acq_rel); \
            goto YIELD_POINT; \
        } \
    } while (0)

#define fiber_yield(fb) \
    do { \
        atomic_thread_fence(memory_order_acq_rel); \
        void * _bp, * _sp; \
        __asm__ __volatile__ ( \
            "mov %%rbp, %0\n\t" \
            "mov %%rsp, %1\n\t" \
            "mov %%rax, %2\n\t" \
            "mov %%rbx, %3\n\t" \
            "mov %%rcx, %4\n\t" \
            "mov %%rdx, %5\n\t" \
            "mov %%rsi, %6\n\t" \
            "mov %%rdi, %7\n\t" \
            : "=r"(_bp), "=r"(_sp), "=r"(fb->registers.ax), "=r"(fb->registers.bx), "=r"(fb->registers.cx), "=r"(fb->registers.dx), "=r"(fb->registers.si), "=r"(fb->registers.di) \
        ); \
        atomic_thread_fence(memory_order_acq_rel); \
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
        atomic_thread_fence(memory_order_acq_rel); \
        fb->status = FIBER_EXITED; \
        return FIBER_EXITED; \
    } while (0)

inline fiber_status fiber_check_status(fiber * fb) {
    return fb->status;
}

extern void fiber_join(fiber * fb);
extern void fiber_detach(fiber *fb);

#endif /* end of include guard FIBER_H */
