import logging
from robot_client import RobotClient, RobotWaypoint, RobotState
from PyQt5.QtCore import QTimer
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

class MainWindow(QMainWindow):
    def __init__(self, robot:RobotClient):
        # Cfg
        super().__init__()
        self.setWindowTitle("Robot Controller")

        # Vars
        self.robot = robot

        # Widgets
        self.control_view = ControlView(MAX_PROGRAMS)
        self.prog_view = ProgramView(N_MOTORS, MIN_PWM, MAX_PWM, WAYPOINT_MIN_IDX, WAYPOINT_MAX_IDX)
        self.run_view = PositionView(N_MOTORS, MOTOR_LABELS)

        # Signals and Slots
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

    def toggle_start_stop(self):
        state, waypoint = self.robot.get_waypoint_current()
        if state == RobotState.IDLE:
            self.prog_view.setDisabled(True)
            self.control_view.setRunning(True)
            self.robot.run()
        elif state == RobotState.RUNNING:
            self.prog_view.setDisabled(False)
            self.control_view.setRunning(False)
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

    simulate = False
    ip = "127.0.0.1" if simulate else "192.168.4.1"
    port = 5555 if simulate else 8080

    app = QApplication(sys.argv)
    window = MainWindow(RobotClient(ip, port))
    window.show()
    app.exec()