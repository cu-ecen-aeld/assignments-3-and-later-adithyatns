#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// Flag set to 1 when SIGINT or SIGTERM is received
extern volatile sig_atomic_t shutdown_requested;

/*
 * Registers handlers for SIGINT and SIGTERM.
 * Returns 0 on success, or -1 on failure.
 */
int register_signal_handlers(void);

#ifdef __cplusplus
}
#endif

#endif // SIGNAL_HANDLER_H
