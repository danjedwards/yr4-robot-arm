import asyncio
import websockets
import socket

# Global variable to track the connected client
connected_client = None

def connect_to_robot(ip, port):
    robot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    robot_socket.connect((ip, port))
    return robot_socket

async def handle_websocket(websocket, path):
    global connected_client

    # Reject new connections if there's already a connected client
    if connected_client is not None:
        await websocket.close(reason="Only one client is allowed at a time.")
        print("Rejected a connection: Only one client allowed.")
        return

    # Assign the connected client
    connected_client = websocket
    print("WebSocket client connected")

    robot_socket = connect_to_robot("192.168.4.1", 8080)

    try:
        while True:
            # Receive a message from the WebSocket client
            message = await websocket.recv()
            print(f"Received from client: {message}")

            # Send the message to the robot via TCP
            robot_socket.sendall(message)
            
            # Receive the robot's response
            response = robot_socket.recv(1024)
            print(f"Received from robot: {response}")

            # Forward the robot's response back to the WebSocket client
            await websocket.send(response)
    except websockets.ConnectionClosed:
        print("WebSocket client disconnected")
    finally:
        # robot_socket.close()
        connected_client = None 

# Start the WebSocket server
async def main(ws_ip, ws_port):
    async with websockets.serve(handle_websocket, ws_ip, ws_port):
        print(f"WebSocket server running on ws://{ws_ip}:{ws_port}")
        await asyncio.Future()

if __name__ == "__main__":
    asyncio.run(main(
        ws_ip="0.0.0.0",
        ws_port=8000
    ))
