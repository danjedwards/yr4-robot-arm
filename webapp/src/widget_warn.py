from PyQt5.QtWidgets import (
    QMessageBox
)

class WarningPopUp(QMessageBox):
    def __init__(self, title, message, information):
        super().__init__()

        self.setIcon(QMessageBox.Warning)
        self.setWindowTitle(title)
        self.setText(message)
        self.setInformativeText(information)
        self.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)
        self.setDefaultButton(QMessageBox.Cancel)

if __name__ == "__main__":
    import sys
    from PyQt5.QtWidgets import QApplication
    app = QApplication(sys.argv)


    test_widget = WarningPopUp(
        "My Warning", 
        "This is my warning message",
        "This is informative text."
    )
    response = test_widget.exec_()

    if response == QMessageBox.Ok:
        print("Okay selected")
    elif response == QMessageBox.Cancel:
        print("Cancel selected")
    else:
        print("Unkown signal pressed!")
