#include "CppUTest/TestHarness.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

extern "C" {
#include "../data_store.h"
}

TEST_GROUP(DataStoreGroup)
{
    int sv[2]; // Socket pair

    void setup() {
        // Ensure the data file does not exist before starting the test
        unlink(DATA_STORE_FILE);
        // Create a bidirectional socket pair for mocking the client connection
        socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    }

    void teardown() {
        close(sv[0]);
        close(sv[1]);
        unlink(DATA_STORE_FILE); // Clean up file after test
    }
};

TEST(DataStoreGroup, BufferGrowsAndWritesOnNewline)
{
    // Write in chunks to simulate dynamic packet buffering
    const char *part1 = "hello ";
    const char *part2 = "world\n";
    write(sv[0], part1, strlen(part1));
    write(sv[0], part2, strlen(part2));

    // Handle the mock client socket (sv[1] acts as the server's connected socket)
    int res = data_store_handle_client(sv[1]);
    CHECK_EQUAL_TEXT(0, res, "Expected data_store_handle_client to succeed");

    // Verify file persistence
    int fd = open(DATA_STORE_FILE, O_RDONLY);
    CHECK_TRUE_TEXT(fd >= 0, "Data store file should have been created");
    char file_buf[128] = {0};
    read(fd, file_buf, sizeof(file_buf));
    close(fd);
    STRCMP_EQUAL("hello world\n", file_buf);

    // Verify it sent the full file content back to the client socket (sv[0])
    char recv_buf[128] = {0};
    ssize_t bytes_recvd = recv(sv[0], recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
    CHECK_TRUE_TEXT(bytes_recvd > 0, "Expected to receive echoed data");
    STRCMP_EQUAL("hello world\n", recv_buf);
}
