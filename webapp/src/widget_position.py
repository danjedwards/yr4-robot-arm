from PyQt5.QtWidgets import (
    QApplication, 
    QWidget,
    QVBoxLayout,
    QTableWidget,
    QHeaderView,
)       

class PositionView(QWidget):
    def __init__(self, n_motors):
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

        # Signals and Slots

        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.table)
        self.setLayout(layout)

if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    test_widget = PositionView(6)

    test_widget.show()
    sys.exit(app.exec_())