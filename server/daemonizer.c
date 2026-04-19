#include "daemonizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int daemonize(void) {
    pid_t pid;

    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        return -1; // Fork failed
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Terminate the parent
    }

    // Create a new SID for the child process
    if (setsid() < 0) {
        return -1;
    }

    // Fork a second time to ensure the daemon cannot acquire a terminal
    pid = fork();
    if (pid < 0) {
        return -1; // Fork failed
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Terminate the first child
    }

    // Change the current working directory to the root directory
    if (chdir("/") < 0) {
        return -1;
    }

    // Close standard file descriptors and redirect to /dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd >= 0) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) {
            close(fd);
        }
    } else {
        return -1;
    }

    return 0;
}
