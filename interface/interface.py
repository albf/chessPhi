#!/usr/bin/env python
# -*- coding: utf-8 -*-

#disclaimer block

# Marcus Botacin
# Alexandre 
# Unicamp 2015 Guido

#import block

import numpy as np
import sys
from PyQt4.QtCore import pyqtSlot
from PyQt4 import QtGui
from PyQt4.QtGui import *
import subprocess

# piece definitions block
pawn=9
castle=7
knight=5
bishop=3
queen=2
king=1

#interface class block

class ChessInterface(QtGui.QWidget):
    def initBoard():
        board = np.zeros(shape=(8,8))
        return board
    def mousePressEvent(self, QMouseEvent):
        print("coming soon")
    def __init__(self):
        super(ChessInterface, self).__init__()
        self.initUI()                               
    def initUI(self):
        print("coming soon")
        board=self.initBoard()


# main and other python stuffs
def main():
    app = QtGui.QApplication(sys.argv)
    ex = ChessInterface()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
