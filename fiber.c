#include "fiber.h"

static fiber * fiber_list_head = NULL;
static fiber * fiber_list_tail = NULL;

// thread start function
int fiber_main_loop(void *a) {
    jmp_buf buf;
    fiber * fb = fiber_list_head;
    while (1) {
        if (fb->status == FIBER_PAUSE) {
            if (setjmp(buf) == 0) {
                // copy fiber frame
                
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
