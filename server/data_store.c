#include "data_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CHUNK_SIZE 1024

void datastore_init(void) {
    if (access(DATA_STORE_FILE, F_OK) == 0) {
        unlink(DATA_STORE_FILE);
    }
}

void datastore_cleanup(void) {
    if (access(DATA_STORE_FILE, F_OK) == 0) {
        unlink(DATA_STORE_FILE);
    }
}

int data_store_handle_client(int client_fd) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    size_t data_len = 0;
    int newline_found = 0;
    char recv_chunk[CHUNK_SIZE];
    ssize_t bytes_received;

    // 1. Read dynamically until a newline is found
    while (!newline_found) {
        bytes_received = recv(client_fd, recv_chunk, CHUNK_SIZE, 0);
        if (bytes_received <= 0) {
            free(buffer);
            return -1; // Connection closed or error before newline
        }

        char *new_buffer = realloc(buffer, buffer_size + bytes_received);
        if (!new_buffer) {
            free(buffer);
            return -1; // Memory allocation failed
        }
        buffer = new_buffer;
        buffer_size += bytes_received;

        // Process the received chunk
        for (ssize_t i = 0; i < bytes_received; i++) {
            buffer[data_len++] = recv_chunk[i];
            if (recv_chunk[i] == '\n') {
                newline_found = 1;
                break; // Stop buffering at the first newline
            }
        }
    }

    // 2. Append the accumulated buffer to the file
    int file_fd = open(DATA_STORE_FILE, O_CREAT | O_APPEND | O_WRONLY, 0644);
    if (file_fd == -1) {
        free(buffer);
        return -1;
    }

    ssize_t bytes_written = write(file_fd, buffer, data_len);
    free(buffer); // Clean up the dynamic buffer
    close(file_fd);

    if (bytes_written != (ssize_t)data_len) {
        return -1;
    }

    // 3. Send the entire file contents back over the socket
    file_fd = open(DATA_STORE_FILE, O_RDONLY);
    if (file_fd == -1) {
        return -1;
    }

    char send_chunk[CHUNK_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, send_chunk, CHUNK_SIZE)) > 0) {
        ssize_t total_sent = 0;
        while (total_sent < bytes_read) {
            ssize_t sent = send(client_fd, send_chunk + total_sent, bytes_read - total_sent, 0);
            if (sent == -1) {
                close(file_fd);
                return -1;
            }
            total_sent += sent;
        }
    }
    
    close(file_fd);
    return 0;
}
