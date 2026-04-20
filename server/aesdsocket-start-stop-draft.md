# aesdsocket-start-stop Draft

This script follows the standard Linux init script structure to manage the `aesdsocket` daemon using `start-stop-daemon`.

```bash
#!/bin/sh

case "$1" in
    start)
        echo "Starting aesdsocket"
        # -S: Start the daemon
        # -n: Process name to look for
        # -a: Path to the executable
        # --: Arguments passed to the daemon (-d for daemon mode)
        start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket -- -d
        ;;
    stop)
        echo "Stopping aesdsocket"
        # -K: Stop the daemon
        # -n: Process name to stop
        # -s: Signal to send (SIGTERM)
        start-stop-daemon -K -n aesdsocket -s SIGTERM
        ;;
    *)
        echo "Usage: $0 {start|stop}"
        exit 1
esac

exit 0
```
