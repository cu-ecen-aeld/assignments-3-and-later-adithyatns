import subprocess
import socket
import time
import pytest
import os
import signal

def test_syslog_connection_logging():
    # Ensure the executable exists
    executable_path = "./aesdsocket"
    assert os.path.exists(executable_path), f"{executable_path} executable not found. Did you run 'make'?"

    # Start the aesdsocket process as a daemon
    process = subprocess.Popen([executable_path, "-d"])
    
    try:
        # Wait for the parent to exit immediately, confirming background execution
        process.wait(timeout=2)
        assert process.returncode == 0, "Launcher parent process should exit with 0."

        # Give the background daemon a moment to finish starting and start listening
        time.sleep(0.5)
        
        # Connect to the server
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect(("127.0.0.1", 9000))
        
        # Send data and wait for echo
        client_socket.sendall(b"hello integration test\n")
        response = client_socket.recv(1024)
        assert b"hello integration test\n" in response
        
        # Close the connection
        client_socket.close()
        
        # Give syslog a moment to write the logs
        time.sleep(0.5)
        
    finally:
        # Terminate the background daemon gracefully via pkill SIGINT
        subprocess.run(["pkill", "-x", "-SIGINT", "aesdsocket"])
        time.sleep(1) # wait for cleanup
        
        # Verify graceful cleanup occurred
        assert not os.path.exists("/var/tmp/aesdsocketdata"), "The data file should have been deleted upon graceful exit."

    # Check syslog using journalctl
    log_check = subprocess.run(
        ["journalctl", "-t", "aesdsocket", "--since", "10 seconds ago"],
        capture_output=True,
        text=True
    )
    logs = log_check.stdout
    
    # Fallback to /var/log/syslog if journalctl doesn't return anything
    if not logs:
        try:
            with open("/var/log/syslog", "r") as f:
                logs = f.read()
        except FileNotFoundError:
            pass

    assert "Accepted connection from 127.0.0.1" in logs, f"Log for accepted connection not found. Logs: {logs}"
    assert "Closed connection from 127.0.0.1" in logs, f"Log for closed connection not found. Logs: {logs}"

