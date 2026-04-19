#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initializes a server socket and binds it to the given port.
 * Returns the socket file descriptor on success, or -1 on failure.
 */
int socket_server_init(const char* port);

/*
 * Starts listening on the given socket file descriptor.
 * Returns 0 on success, or -1 on failure.
 */
int socket_server_listen(int sockfd, int backlog);

/*
 * Accepts an incoming connection on the listening socket.
 * Populates client_ip with the string representation of the client's IP address.
 * Returns the client socket file descriptor on success, or -1 on failure.
 */
int socket_server_accept(int sockfd, char *client_ip, size_t ip_len);

/*
 * Closes the server socket robustly.
 */
void socket_server_close(int sockfd);

#ifdef __cplusplus
}
#endif

#endif // SOCKET_MANAGER_H
