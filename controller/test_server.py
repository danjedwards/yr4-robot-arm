import socket
import time

# ESP32 server configuration
ESP32_IP = "192.168.4.1"
SERVER_PORT = 8080

def connect_to_server():
    """Connect to the ESP32 server."""
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect((ESP32_IP, SERVER_PORT))
        print(f"Connected to server at {ESP32_IP}:{SERVER_PORT}")
        return client_socket
    except ConnectionError as e:
        print(f"Failed to connect to server: {e}")
        return None

def send_and_receive(client_socket, message):
    """Send a message to the server and receive a response."""
    try:
        # Send the message
        client_socket.sendall(message.encode())
        print(f"Sent: {message}")

        # Wait to receive the response
        response = client_socket.recv(1024).decode()
        print(f"Received: {response}")
    except Exception as e:
        print(f"Communication error: {e}")

def main():
    # Connect to the server
    client_socket = connect_to_server()
    if client_socket is None:
        return

    # Example: Sending multiple messages to the server
    messages = ["Hello ESP32!", "How are you?", "This is a test message", "Goodbye!"]
    for msg in messages:
        send_and_receive(client_socket, msg)
        time.sleep(1)  # Wait a bit between messages

    # Close the connection
    client_socket.close()
    print("Connection closed")

if __name__ == "__main__":
    main()
