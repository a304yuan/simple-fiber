#include <stdlib.h>
#include <threads.h>
#include "fiber.h"
#include "simple-ring-buffer/ring_buffer.h"

static thrd_t * thread_list = NULL;
static ring_buffer * fiber_list = NULL;

// thread start function
int fiber_main_loop(void *th) {
    fiber * fb = NULL;
    while (1) {
        if (ring_buffer_read(fiber_list, &fb)) {
            if (fb->next_run <= clock()) {
                int status = fb->func(fb, fb->arg);
                if (status == FIBER_EXITED) {
                    if (fb->defunc) {
                        fb->defunc(fb);
                    }
                    free(fb);
                }
                else {
                    ring_buffer_write(fiber_list, &fb);
                }
            }
            else {
                ring_buffer_write(fiber_list, &fb);
            }
        }
        else {
            thrd_yield();
        }
    }
}

void fiber_init(int threads, long max_fibers) {
    fiber_list = ring_buffer_new(sizeof(fiber*), max_fibers);
    thread_list = malloc(threads * sizeof(thrd_t));
    for (int i = 0; i < threads; i++) {
        thrd_create(thread_list + i, fiber_main_loop, NULL);
    }
}

int fiber_create(fiber_start_func func, void * arg, fiber_deallocator defunc) {
    fiber * fb = malloc(sizeof(fiber));
    fb->func = func;
    fb->defunc = defunc;
    fb->arg = arg;
    fb->status = FIBER_PAUSED;
    fb->next_run = 0;
    fb->yield_point = NULL;
    return ring_buffer_write(fiber_list, &fb);
}
