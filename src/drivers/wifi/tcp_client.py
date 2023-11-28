"""Receives and sends data to the Pico server over TCP.
"""

import time
import socket
_author = "Jurgen Tan"


PICO_IP = "172.20.10.2"  # Change this to the IP of the Pico
TCP_PORT = 4242  # Change this to the port of the Pico server

start_time = time.time()  # Get the current time


def main() -> None:
    """Runs the TCP client.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            print("Connecting...")
            s.connect((PICO_IP, TCP_PORT))
            print(f"Connected to {PICO_IP}:{TCP_PORT}")

            while True:
                # Get the current time
                elapsed_time = time.time() - start_time

                # Create a message with the elapsed time
                message = f"Elapsed Time: {elapsed_time:.2f} seconds\n"

                # Send the message to the server
                s.sendall(message.encode())
                print(f"Sent: {message.strip()}")

                try:
                    response = s.recv(2048)  # Adjust the buffer size as needed
                    response = int.from_bytes(response, "little")
                    print(f"Received: {response}")

                except Exception:
                    print("No response received")

                print("Sleeping...")
                # Sleep for a while
                time.sleep(3)

        except ConnectionRefusedError:
            print("Connection refused. Ensure the Pico server \
                is running and the IP and port are correct.")


if __name__ == "__main__":
    main()
