#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Marcus Botacin
# Alexandre 
# Unicamp 2015 Guido

import numpy as np
import sys
from PyQt4.QtCore import pyqtSlot
from PyQt4 import QtGui
from PyQt4.QtGui import *
import subprocess

class ChessInterface(QtGui.QWidget):
    def mousePressEvent(self, QMouseEvent):
        print("coming soon")
    def __init__(self):
        super(ChessInterface, self).__init__()
        self.initUI()                               
    def initUI(self):
        print("coming soon")

def main():
    app = QtGui.QApplication(sys.argv)
    ex = ChessInterface()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
