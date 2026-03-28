import socket
import time

HOST = '127.0.0.1'  # Localhost (loopback interface)
PORT = 8080         # The port our C++ server will be listening on
FILE_PATH = 'orders.csv'

def run_load_tester():
    print("========================================")
    print(" LSEG Network Load Tester Initiated")
    print("========================================")
    print(f"Target: {HOST}:{PORT}")
    print(f"Payload file: {FILE_PATH}")

    # Pre-load data into RAM
    # We do this so the Python script's hard drive read time (File I/O) doesn't artificially slow down our network throughput benchmark.
    try:
        with open(FILE_PATH, mode='r') as file:
            lines = file.readlines()
    except FileNotFoundError:
        print(f"Error: Could not find {FILE_PATH}. Run the generator script first!")
        return

    # Pop the CSV header so we don't send it to the engine
    if len(lines) > 0 and "Client Order ID" in lines[0]:
        lines.pop(0)

    total_orders = len(lines)
    print(f"Loaded {total_orders:,} orders into RAM.")
    print("Attempting to connect to matching engine...\n")

    # Establish TCP Connection
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            print(">>> Connection Established! Firing payload... <<<")
            
            start_time = time.time()
            
            # Blast the data across the socket
            for line in lines:
                # We encode the string to bytes. 
                # The newline (\n) is critical—it tells the C++ server where the order ends.
                s.sendall(line.encode('utf-8'))
            
            # Send the "Poison Pill"
            # We send a fake EOF order so the C++ server knows to stop listening and shut down cleanly.
            s.sendall(b"EOF,Rose,1,10,10.0\n")
            
            end_time = time.time()
            elapsed_ms = (end_time - start_time) * 1000
            
            print("\n========================================")
            print(" Payload Delivered Successfully")
            print("========================================")
            print(f"Orders Sent: {total_orders:,}")
            print(f"Time Taken:  {elapsed_ms:.2f} ms")
            
            # Calculate messages per second
            if elapsed_ms > 0:
                throughput = total_orders / (elapsed_ms / 1000)
                print(f"Throughput:  {throughput:,.0f} messages/sec")
            print("========================================\n")
            
    except ConnectionRefusedError:
        print(f"CRITICAL FAILURE: Connection refused.")
        print(f"Is your C++ matching engine currently running and listening on port {PORT}?")

if __name__ == "__main__":
    run_load_tester()