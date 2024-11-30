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
    program_index_changed = pyqtSignal(int)
    load_program_clicked = pyqtSignal(int)
    save_program_clicked = pyqtSignal(int, str)

    def __init__(self, max_programs:int=5):
        # Cfg
        super().__init__()

        # Widgets
        self.program_index = QComboBox()
        self.program_index.addItems([f"{i}" for i in range(max_programs)])
        
        self.program_name = QLineEdit("Program")

        self.save_program = QPushButton("Save Program")
        self.load_program = QPushButton("Load Program")

        self.start_stop_button = QPushButton("Start") 

        # Signals and Slots
        self.program_index.currentIndexChanged.connect(self.program_index_changed.emit)
        self.load_program.clicked.connect(lambda: self.load_program_clicked.emit(self.program_index.currentIndex()))
        self.save_program.clicked.connect(lambda: self.save_program_clicked.emit(self.program_index.currentIndex(), self.program_name.text()))
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

            self.widget.program_index_changed.connect(self.test_program_index_changed) 
            self.widget.load_program_clicked.connect(self.test_load_program_clicked) 
            self.widget.save_program_clicked.connect(self.test_save_program_clicked)
            self.widget.toggle_start_stop.connect(self.test_toggle_start_stop)

        def test_program_index_changed(self, index):
            print(f"* Received program_index_changed signal with index {index}")

        def test_load_program_clicked(self, index):
            print(f"* Received load_program_clicked signal with index {index}")

        def test_save_program_clicked(self, index, label):
            print(f"* Received save_program_clicked signal with data ({index}, {label})")

        def test_toggle_start_stop(self):
            print("* Received toggle start/stop signal.")

    app = QApplication(sys.argv)
    test_widget = ControlView()

    tester = SignalTester(test_widget)

    test_widget.show()
    sys.exit(app.exec_())