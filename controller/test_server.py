import socket
import time
import struct

MSG_ERROR = 0
MSG_OKAY = 1

MSG_READ = 0
MSG_WRITE = 1

MSG_IDLE =  0
MSG_PROGRAM =  1
RUNNING =  2

MSG_WAYPOINT_COUNT =  0
MSG_WAYPOINT_IDX =  1
MSG_WAYPOINT_CUR =  2
MSG_PROGRAM_COUNT =  3
MSG_PROGRAM_NAME =  4
MSG_PROGRAM_RW =  5
MSG_RUN = 6

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
        print(f"Received: {response}")
        return parse_msg(response)
    except Exception as e:
        print(f"Communication error: {e}")
        exit()

def construct_msg(rw, mode, cmd, byte_data):
    cfg_byte = (((rw & 1) << 6) | ((mode & 0b111) << 3) | (cmd & 0b111)).to_bytes(1, 'big') 
    return cfg_byte + byte_data if byte_data else cfg_byte

def parse_msg(buf):
    err = not ((buf[0] >> 7) & 1)
    rw = (buf[0] >> 6)  & 1
    state = (buf[0] >> 3) & 0b111
    command = buf[0] & 0b111
    data = buf[1:]
    return err, rw, state, command, data

def main():
    # Connect to the server
    client_socket = connect_to_server()

    if client_socket is None:
        return
    
    # Waypoint Count (Read Only)
    msg = construct_msg(MSG_READ, 0, MSG_WAYPOINT_COUNT, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_WAYPOINT_COUNT
    assert int.from_bytes((data), "little") == 100
    
    input("Hit enter for next test...")

    # Waypoint Index
    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_IDX, (26).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_WRITE
    assert state == MSG_IDLE
    assert command == MSG_WAYPOINT_IDX
    assert int.from_bytes((data), "little") == 26

    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_IDX, (100).to_bytes(1, "little"))
    err, rw, state, command, data =  send_and_receive(client_socket, msg)
    assert err == True

    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_IDX, (0).to_bytes(1, "little"))
    send_and_receive(client_socket, msg)
    msg = construct_msg(MSG_READ, 0, MSG_WAYPOINT_IDX, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_WAYPOINT_IDX
    assert int.from_bytes((data), "little") == 0

    input("Hit enter for next test...")

    # Current Waypoint
    msg = construct_msg(MSG_READ, 0, MSG_WAYPOINT_CUR, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_CUR, bytearray([0xe8, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3]))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_CUR, bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == True
    input("Hit enter for next test...")

    # Program Count (Read Only)
    msg = construct_msg(MSG_READ, 0, MSG_PROGRAM_COUNT, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_PROGRAM_COUNT
    assert int.from_bytes((data), "little") == 5

    input("Hit enter for next test...")

    # Program Name
    for i in range(5):
        err, rw, state, command, data = send_and_receive(client_socket, construct_msg(MSG_READ, 0, MSG_PROGRAM_NAME, (i).to_bytes(1, "little")))
        print(data)
        assert err == False
    
    for i in range(5):
        name = f"Program {i}".encode()
        err, rw, state, command, data = send_and_receive(client_socket, construct_msg(MSG_WRITE, 0, MSG_PROGRAM_NAME, i.to_bytes(1, "little") + name))
        print(data)
        assert err == False
        assert rw == MSG_WRITE
        assert command == MSG_PROGRAM_NAME
        assert data[1:] == name

    input("Hit enter for next test...")

    # Program R/W
    msg = construct_msg(MSG_READ, 0, MSG_PROGRAM_RW, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == True

    msg = construct_msg(MSG_READ, 0, MSG_PROGRAM_RW, (5).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == True

    msg = construct_msg(MSG_READ, 0, MSG_PROGRAM_RW, (0).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    print(data)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_PROGRAM_RW

    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_IDX, (0).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)

    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_CUR, bytearray([0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3]))
    err, rw, state, command, data = send_and_receive(client_socket, msg)

    msg = construct_msg(MSG_WRITE, 0, MSG_PROGRAM_RW, (0).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    
    msg = construct_msg(MSG_WRITE, 0, MSG_WAYPOINT_CUR, bytearray([0xe8, 0x4, 0xe8, 0x4, 0xe8, 0x4, 0xe8, 0x4, 0xe8, 0x4, 0xe8, 0x4]))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False

    msg = construct_msg(MSG_READ, 0, MSG_PROGRAM_RW, (0).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    print(data)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_PROGRAM_RW
    
    msg = construct_msg(MSG_READ, 0, MSG_WAYPOINT_CUR, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_WAYPOINT_CUR
    assert data[0] == 0x3

    # Run
    msg = construct_msg(MSG_READ, 0, MSG_RUN, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_READ
    assert state == MSG_IDLE
    assert command == MSG_RUN

    msg = construct_msg(MSG_WRITE, 0, MSG_RUN, None)
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == True

    msg = construct_msg(MSG_WRITE, 0, MSG_RUN, (1).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_WRITE
    assert state == RUNNING
    assert command == MSG_RUN

    msg = construct_msg(MSG_WRITE, 0, MSG_RUN, (0).to_bytes(1, "little"))
    err, rw, state, command, data = send_and_receive(client_socket, msg)
    assert err == False
    assert rw == MSG_WRITE
    assert state == MSG_IDLE
    assert command == MSG_RUN

    # Close the connection
    client_socket.close()
    print("Connection closed")

if __name__ == "__main__":
    main()