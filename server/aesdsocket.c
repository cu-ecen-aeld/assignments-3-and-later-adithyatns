#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>
#include "socket_manager.h"
#include "data_store.h"
#include "signal_handler.h"
#include "daemonizer.h"

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    char client_ip[INET6_ADDRSTRLEN];
    bool run_as_daemon = false;
    int opt;

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "d")) != -1) {
        if (opt == 'd') {
            run_as_daemon = true;
        }
    }

    openlog("aesdsocket", LOG_PID | LOG_CONS, LOG_USER);

    if (register_signal_handlers() == -1) {
        syslog(LOG_ERR, "Failed to register signal handlers");
        closelog();
        return -1;
    }

    datastore_init();

    server_fd = socket_server_init("9000");
    if (server_fd == -1) {
        syslog(LOG_ERR, "Failed to initialize server socket");
        datastore_cleanup();
        closelog();
        return -1;
    }

    // Daemonize after binding but before listening/accepting, as per requirements
    if (run_as_daemon) {
        if (daemonize() == -1) {
            syslog(LOG_ERR, "Failed to daemonize");
            socket_server_close(server_fd);
            datastore_cleanup();
            closelog();
            return -1;
        }
        syslog(LOG_INFO, "Successfully daemonized process");
    }

    if (socket_server_listen(server_fd, 10) == -1) {
        syslog(LOG_ERR, "Failed to listen on server socket");
        socket_server_close(server_fd);
        datastore_cleanup();
        closelog();
        return -1;
    }

    while (!shutdown_requested) {
        client_fd = socket_server_accept(server_fd, client_ip, sizeof(client_ip));
        if (client_fd == -1) {
            if (errno == EINTR) {
                // Interrupted by signal, loop condition will catch shutdown_requested
                continue;
            }
            syslog(LOG_ERR, "Failed to accept connection");
            continue;
        }

        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        // Receive data until newline, append to file, and echo file back
        if (data_store_handle_client(client_fd) == -1) {
            syslog(LOG_ERR, "Error handling client data for %s", client_ip);
        }

        // Close connection
        close(client_fd);
        syslog(LOG_INFO, "Closed connection from %s", client_ip);
    }

    syslog(LOG_INFO, "Caught signal, exiting");

    socket_server_close(server_fd);
    datastore_cleanup();
    closelog();
    return 0;
}
