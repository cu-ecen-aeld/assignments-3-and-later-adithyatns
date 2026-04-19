#include "signal_handler.h"
#include <stddef.h>

volatile sig_atomic_t shutdown_requested = 0;

static void handle_signal(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        shutdown_requested = 1;
    }
}

int register_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0; // Don't use SA_RESTART so accept() gets interrupted by EINTR
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        return -1;
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        return -1;
    }

    return 0;
}
