import socket
import time
import struct

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
        client_socket.sendall(message)
        print(f"Sent: {message}")

        # Wait to receive the response
        response = client_socket.recv(1024)
        parse_msg(response)
        print(f"Received: {response}")
    except Exception as e:
        print(f"Communication error: {e}")

def construct_msg(rw, mode, cmd, byte_data):
    cfg_byte = ((rw & 1) << 6) | ((mode & 0b111) << 3) | (cmd & 0b111)
    return cfg_byte.to_bytes(1, 'big') + byte_data 

def parse_msg(buf):
    err = (buf[0] >> 7)
    print(err)

def main():
    # Connect to the server
    client_socket = connect_to_server()

    if client_socket is None:
        return
    
    messages = [construct_msg(1, 0, 0, b"0"), construct_msg(0, 0, 0, b"0")]
    for msg in messages:
        pass
        send_and_receive(client_socket, msg)
        time.sleep(1)  # Wait a bit between messages

    # Close the connection
    client_socket.close()
    print("Connection closed")

if __name__ == "__main__":
    main()
    msg = construct_msg(1200, 0, 0, 0, 0, 0)
