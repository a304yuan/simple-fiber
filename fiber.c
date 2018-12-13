#include "fiber.h"
#include "ring_buffer.h"

static thrd_t * thread_list = NULL;
static ring_buffer * fiber_list = NULL;

// thread start function
int fiber_main_loop(void *th) {
    fiber * fb = NULL;
    while (1) {
        if (ring_buffer_read(fiber_list, &fb)) {
            fb->status = FIBER_RUNNING;
            if (fb->func(fb, fb->arg) == FIBER_EXITED) {
                free(fb->frame);
                free(fb);
            }
            else {
                fb->status = FIBER_PAUSE;
                ring_buffer_write(fiber_list, &fb);
            }
        }
        else {
            thrd_yield();
        }
    }
}

void fiber_init(int threads) {
    fiber_list = ring_buffer_new(sizeof(fiber*), 1024);
    thread_list = malloc(threads * sizeof(thrd_t));
    for (int i = 0; i < threads; i++) {
        thrd_create(thread_list + i, fiber_main_loop, NULL);
    }
}

void fiber_create(fiber_start_func func, void * arg) {
    fiber * fb = malloc(sizeof(fiber));
    fb->func = func;
    fb->arg = arg;
    fb->frame_size = 0;
    fb->status = FIBER_PAUSE;
    fb->frame = NULL;
    ring_buffer_write(fiber_list, &fb);
}
