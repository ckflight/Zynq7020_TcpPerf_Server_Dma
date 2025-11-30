import socket
import time
import threading
import os

HOST = "192.168.1.10"   # Zynq IP
PORT = 5001             # iPerf port (TCP_CONN_PORT)
DURATION = 10           # seconds
BUF_SIZE = 32 * 1024   # 256 KB send buffer 512 * 1024 becomes slower 16 is creating problem 32 is best fastest

# Create a large buffer to send repeatedly
data = b"A" * BUF_SIZE


def send_loop(sock, stop_event):
    """Send data as fast as possible in a tight loop."""
    sent_bytes = 0
    mv = memoryview(data)

    while not stop_event.is_set():
        try:
            n = sock.send(mv)
            if n <= 0:
                break
            sent_bytes += n
        except Exception:
            break

    return sent_bytes


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    print(f"Connecting to {HOST}:{PORT} ...")
    sock.connect((HOST, PORT))
    print("Connected. Sending data...")

    # Thread for sending (allows timer-based stop)
    stop_event = threading.Event()
    start = time.time()

    # Start sending in a separate thread
    thread = threading.Thread(target=send_loop, args=(sock, stop_event))
    thread.start()

    # Wait for duration
    time.sleep(DURATION)
    stop_event.set()
    thread.join()

    end = time.time()

    # Total bytes sent is measured by thread
    # but Python threads can't return values directly
    # so let's close socket to finalize
    try:
        sock.shutdown(socket.SHUT_WR)
    except:
        pass
    sock.close()

    duration = end - start
    total_bytes = BUF_SIZE * (duration / 0.000001)   # dummy but replaced below

    print("\nDone.")
    print("IMPORTANT:")
    print("  Zynq UART will show the REAL throughput.")
    print("  Python only pushes, Zynq measures.")

if __name__ == "__main__":
    main()
