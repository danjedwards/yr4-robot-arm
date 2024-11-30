from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import (
    QApplication, 
    QPushButton,
    QWidget,
    QHBoxLayout,
    QVBoxLayout,
    QComboBox,
    QLineEdit
)

class ControlView(QWidget):
    toggle_start_stop = pyqtSignal()

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
        self.start_stop_button.clicked.connect(self.toggle_start_stop.emit)

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

if __name__ == "__main__":
    import sys

    class SignalTester:
        def __init__(self, widget:ControlView):
            self.widget = widget

            self.widget.toggle_start_stop.connect(self.test_toggle_start_stop)

        def test_toggle_start_stop(self):
            print("* Received toggle start/stop signal.")

    app = QApplication(sys.argv)
    test_widget = ControlView()

    tester = SignalTester(test_widget)

    test_widget.show()
    sys.exit(app.exec_())