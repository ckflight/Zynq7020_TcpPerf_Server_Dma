import socket
import time
import os
import sys

# -----------------------------
# Configuration
# -----------------------------
HOST = "192.168.1.10"   # ZC702 IP
PORT = 7               # TCP echo port echo server
DURATION = 10           # seconds to test
PACKET_SIZE = 8192      # bytes per packet
UPDATE_INTERVAL = 1.0   # print status every second
TIMEOUT = 3.0           # socket timeout

# -----------------------------
# Connection setup
# -----------------------------
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(TIMEOUT)

print(f"[INFO] Trying to connect to {HOST}:{PORT} ...")
try:
    sock.connect((HOST, PORT))
except socket.timeout:
    print(f"[ERROR] Connection timeout after {TIMEOUT}s.")
    sys.exit(1)
except Exception as e:
    print(f"[ERROR] Connection failed: {e}")
    sys.exit(1)

print(f"[OK] Connected to {HOST}:{PORT}")
print(f"[INFO] Sending {PACKET_SIZE}-byte packets for {DURATION}s\n")

# -----------------------------
# Test loop
# -----------------------------
payload = os.urandom(PACKET_SIZE)
bytes_sent = 0
bytes_recv = 0
packets = 0
start_time = time.time()
last_update = start_time

while (time.time() - start_time) < DURATION:
    try:
        sock.sendall(payload)
        data = sock.recv(PACKET_SIZE)
    except socket.timeout:
        print("[WARN] recv() timeout – no echo received.")
        continue
    except Exception as e:
        print(f"[ERROR] Socket error: {e}")
        break

    if not data:
        print("[WARN] Connection closed by remote host.")
        break

    bytes_sent += len(payload)
    bytes_recv += len(data)
    packets += 1

    now = time.time()
    if (now - last_update) >= UPDATE_INTERVAL:
        elapsed = now - start_time
        throughput = (bytes_recv * 8) / (elapsed * 1e6)
        print(f"[{elapsed:5.2f}s] packets={packets:5d}, "
              f"rx={bytes_recv/1e6:6.2f} MB, "
              f"throughput={throughput:6.2f} Mbps")
        last_update = now

sock.close()

# -----------------------------
# Results
# -----------------------------
elapsed = time.time() - start_time
throughput_mbps = (bytes_recv * 8) / (elapsed * 1e6)
pps = bytes_recv / PACKET_SIZE / elapsed

print("\n--- Test Summary ---")
print(f"Elapsed Time   : {elapsed:.2f} s")
print(f"Bytes Sent     : {bytes_sent:,}")
print(f"Bytes Received : {bytes_recv:,}")
print(f"Packets/sec    : {pps:.0f}")
print(f"Throughput     : {throughput_mbps:.2f} Mbps")
print("--------------------")
