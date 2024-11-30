import socket

HOST = '127.0.0.1'
PORT = 5555

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind((HOST, PORT))

server_socket.listen(5)
print(f"Server listening on {HOST}:{PORT}")

try:
    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Connection established with {client_address}")

        while True:
            data = client_socket.recv(1024)
            if not data:  # client has disconnected
                print(f"Client {client_address} disconnected.")
                break

            print(f"Received from {client_address}: {data.decode()}")
            
            client_socket.sendall(data)

        client_socket.close()

except KeyboardInterrupt:
    print("\nShutting down the server.")
finally:
    server_socket.close()