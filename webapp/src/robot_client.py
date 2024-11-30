import socket
import logging
import struct
import select
from typing import Tuple
from dataclasses import dataclass, fields, astuple
from enum import IntEnum

@dataclass
class RobotWaypoint:
    BASE: int
    SHOULDER: int
    ELBOW: int
    WRIST_1: int
    WRIST_2: int
    GRIPPER: int

class RobotReadWrite(IntEnum):
    READ = 0
    WRITE = 1

    def __str__(self):
        mapping = {
            RobotReadWrite.READ: "READ",
            RobotReadWrite.WRITE: "WRITE",
        }
        return mapping.get(self, "Unknown")
    
class RobotCommand(IntEnum):
    WAYPOINT_COUNT = 0
    WAYPOINT_INDEX = 1
    WAYPOINT_CURRENT = 2 
    PROGRAM_COUNT = 3
    PROGRAM_NAME = 4
    PROGRAM_RW = 5
    RUN = 6

    def __str__(self):
        mapping = {
            RobotCommand.WAYPOINT_COUNT: "WAYPOINT_COUNT",
            RobotCommand.WAYPOINT_INDEX: "WAYPOINT_INDEX",
            RobotCommand.WAYPOINT_CURRENT: "WAYPOINT_CURRENT",
            RobotCommand.PROGRAM_COUNT: "PROGRAM_COUNT",
            RobotCommand.PROGRAM_NAME: "PROGRAM_NAME",
            RobotCommand.PROGRAM_RW: "PROGRAM_RW",
            RobotCommand.RUN: "RUN", 
        }
        return mapping.get(self, "Unknown")

class RobotState(IntEnum):
    IDLE =  0
    PROGRAM =  1
    RUNNING =  2

    def __str__(self) -> str:
        mapping = {
            RobotState.IDLE: "IDLE",
            RobotState.PROGRAM: "PROGRAM",
            RobotState.RUNNING: "RUNNING",
        }
        return mapping.get(self, "Unkown")

@dataclass
class RobotMessage:
    err: bool
    rw: RobotReadWrite
    state: RobotState
    command: RobotCommand
    data: bytearray

class RobotClient:
    def __init__(self, ip:str, port:int, timeout_s:float=5.0) -> None:
        self.ip = ip
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_timeout(timeout_s)
        self.socket.connect((self.ip, self.port))
        logging.info(f"Connected to the robot on {self.ip}:{self.port}")

    def flush(self) -> None:
        
        self.socket.setblocking(False)
        
        try:
            while True:
                msg = self.__read()
                logging.warning(f"Unread {str(msg.command)} message!")
        except BlockingIOError:
            logging.info("Data flushed")
        except socket.error as e:
            logging.critical(f"Socket error: {e}")

        self.socket.setblocking(True)

    def __read(self) -> RobotMessage:
        buf = self.socket.recv(1024)
        return RobotMessage(
            err=not ((buf[0] >> 7) & 1),
            rw=(buf[0] >> 6) & 1,
            state=(buf[0] >> 3) & 0b111,
            command=buf[0] & 0b111,
            data=buf[1:],
        )

    def __write(self, message: RobotMessage) -> None:
        cfg_byte = (((message.rw & 1) << 6) | ((message.state & 0b111) << 3) | (message.command & 0b111)).to_bytes(1, 'big')
        buf = cfg_byte + message.data if message.data != None else cfg_byte
        readable, writable, exceptional = select.select([self.socket], [self.socket], [self.socket])

        if exceptional:
            logging.critical("Exceptional socket!!")
            return

        if readable:
            self.flush()
            readable, writable, exceptional = select.select([self.socket], [self.socket], [self.socket])

        if not writable:
            logging.critical("Cannot write to the socket!!")
            return

        self.socket.sendall(buf)

    def __get(self, command: RobotCommand) -> int | None:
        self.__write(RobotMessage(None, RobotReadWrite.READ, RobotState.IDLE, command, None))
        msg = self.__read()
        return int.from_bytes(msg.data, "little") if not msg.err else None

    def set_timeout(self, timeout_s:float=None) -> None:
        self.socket.settimeout(timeout_s)

    def get_waypoint_count(self) -> int | None:
        return self.__get(RobotCommand.WAYPOINT_COUNT)
    
    def get_waypoint_index(self) -> int | None:
        return self.__get(RobotCommand.WAYPOINT_INDEX)
  
    def get_program_count(self) -> int | None:
        return self.__get(RobotCommand.PROGRAM_COUNT)      

    def get_waypoint_current(self) -> Tuple[RobotState, RobotWaypoint|None]:
        self.__write(RobotMessage(None, RobotReadWrite.READ, RobotState.IDLE, RobotCommand.WAYPOINT_CURRENT, None))
        msg = self.__read()
        
        if msg.err:
            logging.critical("Message error")
            return
        
        if len(msg.data) != len(fields(RobotWaypoint))*2:
            logging.critical("Incorrect buffer size!!")
            logging.critical(f"Received data buffer: {msg.data}")
            return msg.state, None
        
        return msg.state, RobotWaypoint(*list(struct.unpack("<" + "H" * (len(msg.data) // 2), msg.data)))

    def get_program_name(self, idx) -> str:
        self.__write(RobotMessage(None, RobotReadWrite.READ, RobotState.IDLE, RobotCommand.PROGRAM_NAME, bytearray([idx])))
        msg = self.__read()
        assert msg.err == False
        return msg.data[1:].decode("utf-8")

    def get_program_names(self) -> list[str] | list[None]:
        program_names = {}
        prog_count = self.get_program_count()

        if not prog_count:
            return program_names

        for i in range(prog_count):
            program_names[i] = self.get_program_name(i)
        
        return program_names

    def save_program(self, program_index:int, program_name:str) -> None:
        prog_idx = bytearray([program_index])
        buf = prog_idx + program_name.encode()
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.PROGRAM_NAME, buf))
        msg = self.__read()
        assert msg.err == False 

        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.PROGRAM_RW, prog_idx))
        msg = self.__read()
        assert msg.err == False

    def set_waypoint_index(self, program_index:int) -> None:
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.WAYPOINT_INDEX, bytearray([program_index])))
        msg = self.__read()
        assert msg.err == False
        assert int.from_bytes(msg.data, "little") == program_index 

    def set_waypoint(self, waypoint:RobotWaypoint) -> None:
        buf = struct.pack("<" + "H" * len(fields(RobotWaypoint)), *list(astuple(waypoint)))
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.WAYPOINT_CURRENT, buf))

    def set_program_name(self, program_index:int, program_name:str) -> None:
        buf = bytearray([program_index]) + program_name.encode()
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.PROGRAM_NAME, buf))
        msg = self.__read()
        assert msg.err == False
        assert msg.data == buf

    def load_program(self, program_index:int) -> None:
        self.__write(RobotMessage(None, RobotReadWrite.READ, RobotState.IDLE, RobotCommand.PROGRAM_RW, bytearray([program_index])))
        msg = self.__read()
        assert msg.err == False

    def run(self) -> None:
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.RUN, bytearray([0x01])))
        msg = self.__read()
        assert msg.err == False

    def stop(self) -> None:
        self.__write(RobotMessage(None, RobotReadWrite.WRITE, RobotState.IDLE, RobotCommand.RUN, bytearray([0x00])))
        msg = self.__read()
        assert msg.err == False

if __name__ == "__main__":
    robot = RobotClient("192.168.4.1", 8080)
    print("* Connected to the Robot")

    w_count = robot.get_waypoint_count()
    w_idx = robot.get_waypoint_index()
    p_count = robot.get_program_count()
    state, w_cur = robot.get_waypoint_current()
    p_names = robot.get_program_names()
    
    # robot.save_program()
    # robot.set_waypoint_index()
    # robot.set_waypoint()
    # robot.set_program_name()
    # robot.load_program()
    # robot.run()
    # robot.stop()

    print(f"Waypoint count: {w_count}")
    print(f"Waypoint Index: {w_idx}")
    print(f"Current Waypoint: {w_cur}") 
    print(f"Program count: {p_count}")
    print(f"Program Names: {p_names}")