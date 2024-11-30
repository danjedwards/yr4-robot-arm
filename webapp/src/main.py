from robot_client import RobotClient, RobotWaypoint
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import (
    QApplication, 
    QMainWindow,
    QWidget,
    QHBoxLayout,
    QVBoxLayout,
)

from widget_control import ControlView
from widget_position import PositionView
from widget_program import ProgramView

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
        self.run_view = PositionView(N_MOTORS)

        # Signals and Slots

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

    def send_request(self):
        print("send")
        self.timer.stop()

if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    window = MainWindow(RobotClient("127.0.0.1", 5555))
    window.show()
    app.exec()