import logging
import socket
import time
from robot_client import RobotClient, RobotWaypoint, RobotState
from PyQt5.QtCore import QObject, pyqtSignal, QThread
from PyQt5.QtWidgets import (
    QApplication, 
    QMainWindow,
    QWidget,
    QHBoxLayout,
    QVBoxLayout,
    QMessageBox,
)

from widget_control import ControlView
from widget_position import PositionView
from widget_program import ProgramView
from widget_warn import WarningPopUp

MOTOR_LABELS = [
    "Base",
    "Shoulder",
    "Elbow",
    "Wrist 1",
    "Wrist 2",
    "Gripper"
]

N_MOTORS = len(MOTOR_LABELS)
MIN_PWM = 410
MAX_PWM = 2048
MAX_PROGRAMS = 5

WAYPOINT_MAX_IDX = 99
WAYPOINT_MIN_IDX = 0

class ListenThread(QObject):
    new_data = pyqtSignal(int, RobotWaypoint)
    finished = pyqtSignal()

    def __init__(self, robot:RobotClient):
        super().__init__()
        self.robot = robot

    def run(self):
        logging.info("Listen thread started")
        self.robot.set_timeout(0.2)
        self.running = True
        while self.running:
            try:
                state, waypoint = self.robot.get_waypoint_current()
                # This needs a re-think :/
                # index = self.robot.get_waypoint_index()
                index = 0
                
                if state != RobotState.RUNNING:
                    self.running = False

                if waypoint == None:
                    logging.critical("No waypoint returned")
                    continue

                self.new_data.emit(index, waypoint)
            except socket.timeout:
                logging.info("Socket timeout")
        
        self.robot.flush()
        self.robot.set_timeout(5.0)
        self.finished.emit()
        logging.info("Listen thread finished")

class ListenThreadWrapper(QObject):
    new_data = pyqtSignal(int, RobotWaypoint)
    finished = pyqtSignal()

    def __init__(self, robot:RobotClient):
        super().__init__()
        self.robot = robot
        self.worker = None
        self.worker_thread = None

    def is_running(self):
        return self.worker_thread.isRunning() if self.worker_thread else None 

    def start(self):
        self.worker = ListenThread(self.robot)
        self.worker_thread = QThread()
        self.worker.moveToThread(self.worker_thread)
        self.worker_thread.started.connect(self.worker.run)
        self.worker.finished.connect(self.stop)
        self.worker.finished.connect(self.finished)
        self.worker.new_data.connect(self.new_data.emit)
        self.worker_thread.start()

    def stop(self):
        if not self.worker_thread:
            return
        self.worker.running = False
        self.worker_thread.quit()
        self.worker_thread.wait()
        self.worker_thread.deleteLater()
        self.worker_thread = None
        self.worker = None

class MainWindow(QMainWindow):
    def __init__(self, robot:RobotClient):
        # Cfg
        super().__init__()
        self.setWindowTitle("Robot Controller")

        # Vars
        self.robot = robot
        self.listen_thread_wrapper = ListenThreadWrapper(self.robot)

        # Widgets
        self.control_view = ControlView(MAX_PROGRAMS)
        self.prog_view = ProgramView(N_MOTORS, MIN_PWM, MAX_PWM, WAYPOINT_MIN_IDX, WAYPOINT_MAX_IDX)
        self.run_view = PositionView(N_MOTORS, MOTOR_LABELS)

        # Signals and Slots
        self.listen_thread_wrapper.new_data.connect(print)
        self.listen_thread_wrapper.finished.connect(lambda: self.enable_controls(True))

        self.control_view.toggle_start_stop.connect(self.toggle_start_stop)
        self.control_view.program_index_changed.connect(self.program_index_changed)
        self.control_view.load_program_clicked.connect(self.load_program)
        self.control_view.save_program_clicked.connect(self.save_program)
        
        self.prog_view.waypoint_index_changed.connect(self.waypoint_index_changed)
        self.prog_view.set_waypoint_clicked.connect(self.set_waypoint)

        # Layout
        layout = QHBoxLayout()
        layout.addWidget(self.prog_view)
        layout.addWidget(self.run_view)
        container = QWidget()
        container.setLayout(layout)

        layout = QVBoxLayout()
        layout.addWidget(self.control_view)
        layout.addWidget(container)
        central_container = QWidget()
        central_container.setLayout(layout)
        self.setCentralWidget(central_container)

    def enable_controls(self, enable):
        self.prog_view.setDisabled(not enable)
        self.control_view.setRunning(enable)

    def toggle_start_stop(self):
        if self.listen_thread_wrapper.is_running():
            self.listen_thread_wrapper.stop()
            
            while self.listen_thread_wrapper.is_running():
                time.sleep(0.1)
        
        state, waypoint = self.robot.get_waypoint_current()
        
        if state == RobotState.IDLE:
            self.enable_controls(False)
            self.robot.run()
            self.listen_thread_wrapper.start()
        elif state == RobotState.RUNNING:
            self.enable_controls(True)
            self.robot.stop()
        else:
            logging.critical("Unkown Robot State!")
            exit(-1)

    def program_index_changed(self, idx):
        program_name = self.robot.get_program_name(idx)
        if not program_name:
            logging.critical(f"Could not read the program name at index {idx}")
            exit(-1)
        self.control_view.program_name.setText(program_name)

    def load_program(self, idx):
        popup = WarningPopUp(
            "Load Program Warning",
            f"Are you sure you want to load program {idx}",
            "Any unsaved changes will be lost if you load the program"
        )

        if popup.exec_() != QMessageBox.Ok:
            return
        
        self.robot.load_program(idx)

    def save_program(self, idx, label):
        popup = WarningPopUp(
            "Save Program Warning",
            f"Are you sure you want to save the current program to program index {idx}",
            f"The current program at index {idx} will be overwritten!"
        )

        if popup.exec_() != QMessageBox.Ok:
            return
        
        self.robot.save_program(idx, label)

    def waypoint_index_changed(self, idx):
        self.robot.set_waypoint_index(idx)

    def set_waypoint(self, idx, waypoint:RobotWaypoint):
        self.robot.set_waypoint(waypoint)

if __name__ == "__main__":
    import sys

    logging.basicConfig(level=logging.DEBUG)

    simulate = False
    ip = "127.0.0.1" if simulate else "192.168.4.1"
    port = 5555 if simulate else 8080

    app = QApplication(sys.argv)
    window = MainWindow(RobotClient(ip, port))
    window.show()
    app.exec()