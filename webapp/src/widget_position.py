from dataclasses import fields
from PyQt5.QtWidgets import (
    QApplication, 
    QWidget,
    QVBoxLayout,
    QTableWidget,
    QTableWidgetItem,
    QHeaderView,
)

from robot_client import RobotWaypoint

class PositionView(QWidget):
    def __init__(self, n_motors, motor_labels):
        # Cfg
        super().__init__()

        # Vars
        table_headers = ["Motor", "Current Position", "Target Position"]

        # Widgets
        self.table = QTableWidget()
        self.table.setRowCount(n_motors)
        self.table.setColumnCount(len(table_headers))
        self.table.setHorizontalHeaderLabels(table_headers)

        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.verticalHeader().setSectionResizeMode(QHeaderView.Stretch)

        for i, lbl in enumerate(motor_labels):
            self.table.setItem(i, 0, QTableWidgetItem(lbl))

        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.table)
        self.setLayout(layout)

    def update_current_position(self, position:RobotWaypoint):
        for i, f in enumerate(fields(RobotWaypoint)):
            self.table.setItem(i, 1, QTableWidgetItem(f"{getattr(position, f.name)}"))

    def update_target_waypoint(self, target:RobotWaypoint):
        for i, f in enumerate(fields(RobotWaypoint)):
            self.table.setItem(i, 2, QTableWidgetItem(f"{getattr(target, f.name)}"))

if __name__ == "__main__":
    import sys
    from PyQt5.QtCore import QTimer
    import random

    app = QApplication(sys.argv)

    labels = []
    for f in fields(RobotWaypoint):
        labels.append(f.name)

    test_widget = PositionView(6, labels)

    current_position = RobotWaypoint(*[1000]*6)
    target_waypoint = RobotWaypoint(*[1000]*6)

    def update_fields():
        for field in fields(RobotWaypoint):
            setattr(current_position, field.name, random.randint(500, 2100))
            setattr(target_waypoint, field.name, random.randint(500, 2100))

        test_widget.update_current_position(current_position)
        test_widget.update_target_waypoint(target_waypoint)

    timer = QTimer()
    timer.timeout.connect(update_fields)
    timer.start(100)

    test_widget.show()
    sys.exit(app.exec_())