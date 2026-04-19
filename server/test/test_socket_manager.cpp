#include "CppUTest/TestHarness.h"
#include <unistd.h>

extern "C" {
#include "../socket_manager.h"
}

TEST_GROUP(SocketManagerGroup)
{
    int sockfd;

    void setup() {
        sockfd = -1;
    }

    void teardown() {
        // Ensure the socket is closed after the test to free the port
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
};

TEST(SocketManagerGroup, CanListenOnPort)
{
    sockfd = socket_server_init("9000");
    CHECK_TRUE_TEXT(sockfd >= 0, "Expected socket_server_init to succeed");
    
    int res = socket_server_listen(sockfd, 5);
    CHECK_EQUAL_TEXT(0, res, "Expected socket_server_listen to return 0 on success");
}
