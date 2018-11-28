#include "fiber.h"

// simple spin lock
static atomic_flag _lock = ATOMIC_FLAG_INIT;
static fiber * fiber_list_head = NULL;
static thrd_t * thread_list = NULL;

static void lock() {
    while(atomic_flag_test_and_set_explicit(&_lock, memory_order_acquire));
}

static void unlock() {
    atomic_flag_clear_explicit(&_lock, memory_order_release);
}

// thread start function
int fiber_main_loop(void *th) {
    fiber * fb = fiber_list_head;
    while (1) {
        if (fb && fb->status == FIBER_PAUSE) {
            fb->func(fb, fb->arg);
            fb = fb->next;
        }
        else {
            thrd_yield();
        }
    }
}

void fiber_init(int threads) {
    thread_list = malloc(threads * sizeof(thrd_t));
    for (int i = 0; i < threads; i++) {
        thrd_create(thread_list + i, fiber_main_loop, NULL);
    }
}

fiber * fiber_create(fiber_start_func func, void * arg) {
    fiber * fb = malloc(sizeof(fiber));
    fb->func = func;
    fb->arg = arg;
    fb->frame_size = 0;
    fb->status = FIBER_PAUSE;
    fb->frame = NULL;
    if (fiber_list_head) {
        fb->next = fiber_list_head->next;
        fiber_list_head->next = fb;
    }
    else {
        fiber_list_head = fb;
        fb->next = fb;
    }
    return fb;
}
