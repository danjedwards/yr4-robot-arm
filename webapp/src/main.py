import sys
from robot_client import RobotClient, RobotWaypoint
from PyQt5.QtCore import (
    QObject,
    QTimer,
)
from PyQt5.QtWidgets import (
    QApplication, 
    QMainWindow,
    QLabel,
    QPushButton,
    QWidget,
    QLayout,
    QHBoxLayout,
    QVBoxLayout,
    QSlider,
    QSpinBox,
    QTableWidget,
    QTableWidgetItem,
    QHeaderView,
    QComboBox,
    QLineEdit
)

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

WAYPOINT_MAX_IDX = 99
WAYPOINT_MIN_IDX = 0

class ControlView(QWidget):
    def __init__(self, max_programs:int=5):
        # Cfg
        super().__init__()

        # Vars

        # Widgets
        self.program_index = QComboBox()
        self.program_index.addItems([f"{i}" for i in range(max_programs)])
        
        self.program_name = QLineEdit("Program")

        self.save_program = QPushButton("Save Program")
        self.load_program = QPushButton("Load Program")

        self.start_stop_button = QPushButton("Start") 

        # Signals and Slots

        # Layout
        layout = QHBoxLayout()
        layout.addWidget(self.program_index)
        layout.addWidget(self.program_name)

        program_label_container = QWidget()
        program_label_container.setLayout(layout)

        layout = QHBoxLayout()
        layout.addWidget(self.load_program)
        layout.addWidget(self.save_program)
        save_load_container = QWidget()
        save_load_container.setLayout(layout)

        layout = QHBoxLayout()
        layout.addWidget(self.start_stop_button)
        start_stop_container = QWidget()
        start_stop_container.setLayout(layout)

        layout = QVBoxLayout()
        layout.addWidget(program_label_container)
        layout.addWidget(save_load_container)
        layout.addWidget(start_stop_container)

        self.setLayout(layout)


class ProgramView(QWidget):
    def __init__(self):
        # Cfg
        super().__init__()

        # Vars

        # Widgets
        self.sliders = []
        for i in range(N_MOTORS):
            slider = QSlider()
            slider.setMinimum(MIN_PWM)
            slider.setMaximum(MAX_PWM)
            slider.setSingleStep(1)
            self.sliders.append(slider)

        self.waypoint_index = QSpinBox()
        self.waypoint_index.setMinimum(WAYPOINT_MIN_IDX)
        self.waypoint_index.setMaximum(WAYPOINT_MAX_IDX)
        
        self.set_waypoint = QPushButton("Set Waypoint")

        # Signals and Slots

        # Layout
        layout = QHBoxLayout()
        for s in self.sliders: layout.addWidget(s)
        slider_container = QWidget()
        slider_container.setLayout(layout)

        layout = QHBoxLayout()
        layout.addWidget(QLabel("Index"))
        layout.addWidget(self.waypoint_index)
        layout.addWidget(self.set_waypoint)
        ctrl_container = QWidget()
        ctrl_container.setLayout(layout)

        layout = QVBoxLayout()
        layout.addWidget(ctrl_container)
        layout.addWidget(slider_container)

        self.setLayout(layout)
        

class PositionView(QWidget):
    def __init__(self):
        # Cfg
        super().__init__()

        # Vars
        table_headers = ["Motor", "Current Position", "Target Position"]

        # Widgets
        self.table = QTableWidget()
        self.table.setRowCount(N_MOTORS)
        self.table.setColumnCount(len(table_headers))
        self.table.setHorizontalHeaderLabels(table_headers)

        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.verticalHeader().setSectionResizeMode(QHeaderView.Stretch)

        # Signals and Slots

        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.table)
        self.setLayout(layout)

class MainWindow(QMainWindow):
    def __init__(self, robot:RobotClient):
        # Cfg
        super().__init__()
        self.setWindowTitle("Robot Controller")

        # Vars
        self.robot = robot

        # Widgets
        self.control_view = ControlView()
        self.prog_view = ProgramView()
        self.run_view = PositionView()
        self.timer = QTimer()

        # Signals and Slots
        self.timer.timeout.connect(self.send_request)

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
    app = QApplication(sys.argv)
    window = MainWindow(RobotClient("127.0.0.1", 5555))
    window.show()
    app.exec()