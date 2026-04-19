# Project Steps

This document tracks the steps taken to set up and develop the `aesdsocket` project.

## Completed Steps

1. **Initial Project Investigation:**
   - Listed files in the directory to understand the current state.
   - Identified the need for source files and a testing framework.

2. **Created Main Application File:**
   - Created a placeholder `aesdsocket.c` file with a standard `main` function to serve as the entry point for the application.

3. **Set Up CppUTest Framework:**
   - Created `test/test_runner.cpp` which utilizes `CommandLineTestRunner::RunAllTests` as the main entry point for the test suite.
   - Created `test/dummy_test.cpp` with a simple test group and test case (`STRCMP_EQUAL`) to verify the CppUTest configuration is working.

4. **Created a Comprehensive `Makefile`:**
   - **Default Targets:** Added `all` and `default` targets to build the `aesdsocket` executable.
   - **Cross-Compilation:** Integrated the `CROSS_COMPILE` variable (e.g., `make CROSS_COMPILE=aarch64-none-linux-gnu-`) for the `CC` and `CXX` compilers.
   - **Testing Target:** Added a `test` target that automatically discovers `.cpp` files in the `test/` directory, compiles them with C++ compiler, links them against CppUTest libraries, and runs the resulting `unit_tests` executable.
   - **Dynamic Library Linking:** Utilized `pkg-config` to robustly locate `cpputest` CFLAGS and LDFLAGS, falling back to `/usr/local` defaults if not found.
   - **Dependency Tracking:** Added `-MMD -MP` flags to automatically generate and include `.d` files, ensuring reliable rebuilds when header files change.
   - **Clean Target:** Added a `clean` target to safely remove executables, object files, and dependency files.

5. **Implemented Socket Manager:**
   - Created the `socket_manager.h` interface exposing `socket_server_init(const char* port)`.
   - Implemented `socket_manager.c` handling socket creation, `setsockopt` for `SO_REUSEADDR`, and binding using `getaddrinfo`.
   - Created `test/test_socket_manager.cpp` to verify that `socket_server_init("9000")` successfully returns a valid socket descriptor.
   - Restructured the `Makefile` to separate application sources from library sources, allowing the test suite to seamlessly link `socket_manager.o`.

6. **Implemented Listen and Accept:**
   - Added `socket_server_listen` and `socket_server_accept` to `socket_manager.h`.
   - Implemented both in `socket_manager.c`. The accept function uses `inet_ntop` to populate the caller's buffer with the string representation of the incoming client's IP.
   - Refactored `test/test_socket_manager.cpp` to include a test verifying that `socket_server_listen` returns successfully (0) on a bound socket.
   - Updated `aesdsocket.c` to enter an infinite loop. It now calls `socket_server_accept`, uses `syslog` to log "Accepted connection from %s", immediately closes the client socket (as a temporary measure), and logs "Closed connection from %s".

7. **Verification:**
   - Ran `make clean && make test` successfully to verify the CppUTest test suite and the listen functionality.

8. **System-Level Testing (Pytest):**
   - Verified that `pytest` is installed in the local Python environment.
   - Created `test/test_system_logging.py` to automate end-to-end system testing.
   - The test script launches `./aesdsocket`, connects to port 9000 using Python's `socket` module, immediately closes the connection, and verifies via `journalctl` (with a `/var/log/syslog` fallback) that both the "Accepted connection" and "Closed connection" syslog events are correctly recorded with the target IP `127.0.0.1`.
   - The test script uses `process.terminate()` to perform a graceful shutdown of the background server post-test.
   - Ran `python3 -m pytest test/test_system_logging.py` successfully.

9. **Implemented DataStore:**
   - Created `data_store.h` establishing the `data_store_handle_client` interface.
   - Implemented `data_store.c` capable of dynamic memory allocation using `malloc`/`realloc` for packet buffering. The buffer reads from the client until a `\n` is detected, appending the content to `/var/tmp/aesdsocketdata`, and finally echoing the entire file back over the socket.
   - Added `test/test_data_store.cpp` using `socketpair(AF_UNIX)` to effectively mock the socket connection and verify buffer growth and newline behavior without establishing an actual internet connection.
   - Updated `aesdsocket.c` to delegate incoming socket payload parsing to `data_store_handle_client(client_fd)`.
   - Updated the `test/test_system_logging.py` integration test to send dummy data, verify the echo, and confirm normal log strings. All unit tests and system tests pass.

10. **Refined Resource Management:**
   - Updated `data_store.h` and `data_store.c` to include `datastore_init` and `datastore_cleanup`. `datastore_init` checks for the existence of the data file using `access()` before unlinking it, ensuring a clean state. `datastore_cleanup` handles the file removal on shutdown.
   - Updated `socket_manager.h` and `socket_manager.c` to include `socket_server_close` for robust socket management.
   - Integrated these functions into `aesdsocket.c` to ensure a consistent lifecycle for all system resources.
   - Verified that all unit tests and system tests pass after the refactoring.

11. **Implemented Signal Handling:**
   - Created `signal_handler.h` and `signal_handler.c` to encapsulate signal handling using `sigaction`.
   - Defined `volatile sig_atomic_t shutdown_requested` set to `1` when `SIGINT` or `SIGTERM` are caught.
   - Updated `aesdsocket.c` to replace its infinite `while (1)` loop with `while (!shutdown_requested)` and handled `errno == EINTR` failures in `accept()` seamlessly.
   - Modified the Pytest integration in `test_system_logging.py` to send `signal.SIGINT` instead of `terminate()` and explicitly verify that the cleanup operations correctly deleted `/var/tmp/aesdsocketdata`.
   - Built and validated all logic successfully.

12. **Implemented Daemonization:**
   - Created `daemonizer.h` and `daemonizer.c` encapsulating the daemonization process.
   - The `daemonize()` function performs the standard Linux double-fork, calls `setsid()`, changes the working directory to `/`, and safely redirects standard streams (`stdin`, `stdout`, `stderr`) to `/dev/null`.
   - Updated `aesdsocket.c` to parse the `-d` command-line argument using `getopt`. 
   - Positioned the daemonization to occur strictly *after* `socket_server_init` (bind phase) and *before* `socket_server_listen` (accept phase), satisfying architectural requirements.
   - Updated the integration test in `test_system_logging.py` to start the process with `-d`, assert the parent process exits quickly (`returncode == 0`), and utilized `pkill` to safely terminate the detached background process.

## Summary

The `aesdsocket` server now supports dynamic packet buffering, persistent data storage (`/var/tmp/aesdsocketdata`), strict syslog integration, robust graceful shutdown via `SIGINT`/`SIGTERM`, and standard double-fork daemonization via the `-d` argument. The full stack passes all internal CppUTest unit suites and Python end-to-end integration boundaries!
