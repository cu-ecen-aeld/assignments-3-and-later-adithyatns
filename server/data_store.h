#ifndef DATA_STORE_H
#define DATA_STORE_H

#ifdef __cplusplus
extern "C" {
#endif

// The path where incoming packets are appended
#define DATA_STORE_FILE "/var/tmp/aesdsocketdata"

/*
 * Initializes the data store, ensuring a clean state.
 */
void datastore_init(void);

/*
 * Cleans up the data store, removing the data file.
 */
void datastore_cleanup(void);

/*
 * Reads from client_fd until a newline character is received.
 * Uses a dynamically allocated and resized buffer to handle the incoming data.
 * Once a newline is found, the accumulated buffer is appended to DATA_STORE_FILE.
 * Then reads the entire DATA_STORE_FILE and sends its contents back to client_fd.
 * Returns 0 on success, or -1 on failure.
 */
int data_store_handle_client(int client_fd);

#ifdef __cplusplus
}
#endif

#endif // DATA_STORE_H
