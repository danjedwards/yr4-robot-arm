import sys
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtWidgets import (
    QApplication, 
    QMainWindow, 
    QPushButton
)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Robot Controller")
        btn = QPushButton("Test Button")
        self.setCentralWidget(btn)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    app.exec()