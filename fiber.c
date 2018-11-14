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
            if (setjmp(thrd->buf) == 0) {
                fb->status = FIBER_RUNNING;
                fb->func(fb, thrd->buf, fb->arg);
            }
        }
        else if (fb->status == FIBER_PAUSE) {
            if (setjmp(thrd->buf) == 0) {
                // copy fiber frame
                void * sp;
                __asm__(
                    "mov %%rsp, %0\n\t"
                    : "=r"(sp)
                );
                void * dest = sp - 2 * sizeof(void*) - fb->frame_size;
                memcpy(dest, fb->frame, fb->frame_size);
                fb->status = FIBER_RUNNING;
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
