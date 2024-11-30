from robot_client import RobotWaypoint
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import (
    QApplication, 
    QLabel,
    QPushButton,
    QWidget,
    QHBoxLayout,
    QVBoxLayout,
    QSlider,
    QSpinBox,
)

class ProgramView(QWidget):
    waypoint_index_changed = pyqtSignal(int)
    set_waypoint_clicked = pyqtSignal(int, RobotWaypoint)

    def __init__(self, n_motos, slider_min, slider_max, waypoint_min, waypoint_max):
        # Cfg
        super().__init__()

        # Widgets
        self.sliders = []
        for i in range(n_motos):
            slider = QSlider()
            slider.setMinimum(slider_min)
            slider.setMaximum(slider_max)
            slider.setValue(slider_min)
            slider.setSingleStep(1)
            self.sliders.append(slider)

        self.waypoint_index = QSpinBox()
        self.waypoint_index.setMinimum(waypoint_min)
        self.waypoint_index.setMaximum(waypoint_max)
        
        self.set_waypoint = QPushButton("Set Waypoint")

        # Signals and Slots
        self.waypoint_index.valueChanged.connect(self.waypoint_index_changed.emit)
        self.set_waypoint.clicked.connect(
            lambda: self.set_waypoint_clicked.emit(
                self.waypoint_index.value(),
                RobotWaypoint(*[s.value() for s in self.sliders])
            )
        )

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
        
if __name__ == "__main__":
    import sys

    class SignalTester:
        def __init__(self, widget:ProgramView):
            self.widget = widget

            self.widget.waypoint_index_changed.connect(self.test_waypoint_index_changed)
            self.widget.set_waypoint_clicked.connect(self.test_set_waypoint_clicked)

        def test_waypoint_index_changed(self, idx:int):
            print(f"* Received index signal change to {idx}")

        def test_set_waypoint_clicked(self, idx:int, waypoint:RobotWaypoint):
            print(f"* Received set waypoint signal for index {idx} and waypoint: {waypoint}")

    app = QApplication(sys.argv)
    test_widget = ProgramView(6, 100, 2000, 0, 99)

    tester = SignalTester(test_widget)

    test_widget.show()
    sys.exit(app.exec_())