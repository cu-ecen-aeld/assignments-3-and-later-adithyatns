#ifndef DAEMONIZER_H
#define DAEMONIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Daemonizes the current process.
 * Performs a double fork, setsid, changes working directory to '/',
 * and redirects stdin, stdout, and stderr to /dev/null.
 * Returns 0 on success, or -1 on failure.
 */
int daemonize(void);

#ifdef __cplusplus
}
#endif

#endif // DAEMONIZER_H
