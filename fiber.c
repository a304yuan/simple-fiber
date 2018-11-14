#include "fiber.h"

static fiber * fiber_list_head = NULL;
static fiber * fiber_list_tail = NULL;
static thread * thread_list = NULL;

// thread start function
int fiber_main_loop(void *th) {
    thread * thrd = th;
    fiber * fb = fiber_list_head;
    while (1) {
        if (fb->status == FIBER_READY) {
            // first run
            fb->func(fb, thrd->buf, fb->arg);
        }
        else if (fb->status == FIBER_PAUSE) {
            if (setjmp(buf) == 0) {
                // copy fiber frame
                __asm__(
                    "mov %%rbp, %0\n\t"
                    "mov %%rsp, %1\n\t"
                    : "=r"(thrd->bp), "=r"(thrd->sp)
                );
                longjmp(fb->buf, FIBER_RUNNING);
            }
        }
        fb = fb->next;
    }
}

void fiber_init(int threads) {

}

fiber * fiber_create(fiber_start_func fun, void * arg) {

}

void fiber_join(fiber * fb) {

}

void fiber_detach(fiber *fb) {

}
