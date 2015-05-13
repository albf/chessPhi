#!/usr/bin/python
# -*- coding: utf-8 -*-

import numpy as np
import sys
from PyQt4.QtCore import pyqtSlot
from PyQt4 import QtGui
from PyQt4.QtGui import *
import subprocess

tabuleiro = np.zeros(shape=(8,8))

@pyqtSlot()
def on_click():
    w = QWidget()
    command="../teste"+" 1 "+' '.join(map(str,(map(int,tabuleiro.reshape(64)))))
    x= subprocess.check_output(command,shell=True)
    r=str(x).split()
    l=len(r)
    s="mova a peça "+r[l-7]+" para "+r[l-5]+","+r[l-4]+".Chance "+r[l-1]
    #print(s)
    QMessageBox.information(w, "Hint", s)

class Example(QtGui.QWidget):
    
    def mousePressEvent(self, QMouseEvent):
                posy=QMouseEvent.pos().x()
                posx=QMouseEvent.pos().y()
                btnx = 8 * posx / 400
                btny = 8 * posy / 400
                w = QWidget()
                QMessageBox.information(w, "Click", "Click em "+str(btnx)+","+str(btny))

                #print(posx,posy)

    def __init__(self):
        super(Example, self).__init__()
        
        self.initUI()
        
    def initUI(self):
       
        grid = QtGui.QGridLayout()
        self.setLayout(grid)

        white=cv2.imread("white.png")
        black=cv2.imread("black.png")
        r_black=cv2.imread("r_black.png")
        b_black=cv2.imread("b_black.png")


        for i in xrange(0,8):
            if(i<3):
                if(i%2==0):
                    for j in xrange(1,8,2):
                        tabuleiro[i][j]=2
                else:
                    for j in xrange(0,8,2):
                        tabuleiro[i][j]=2
            if(i>4):
                if(i%2==0):
                    for j in xrange(1,8,2):
                        tabuleiro[i][j]=3
                else:
                    for j in xrange(0,8,2):
                        tabuleiro[i][j]=3
            if(i==3):
                for j in xrange(0,8):
                    if(j%2==0):
                        tabuleiro[i][j]=1
            if(i==4):
                for j in xrange(0,8):
                    if(j%2==1):
                        tabuleiro[i][j]=1


        positions = [(i,j) for i in range(8) for j in range(8)]
        
        #for position in positions:
        for i in xrange(0,8):
            for j in xrange(0,8):

                label = QLabel()

                if tabuleiro[i][j]==0:
                    pixmap = QPixmap("white.png")
                if tabuleiro[i][j]==1:
                    pixmap = QPixmap("black.png")
                if tabuleiro[i][j]==2:
                    pixmap = QPixmap("r_black.png")
                if tabuleiro[i][j]==3:
                    pixmap = QPixmap("b_black.png")

                label.setPixmap(pixmap)
                label.show()
                #grid.addWidget(label, *position)
                grid.addWidget(label,i,j)

   
    

        w = QWidget()
        btn = QPushButton('Quit!', w)
        btn.setToolTip('Click to exit!')
        btn.clicked.connect(exit)
        btn.resize(btn.sizeHint())
        grid.addWidget(btn,0,8)
        btn = QPushButton('Hint!', w)
        btn.setToolTip('Click to get a hint!')
        btn.clicked.connect(on_click)
        btn.resize(btn.sizeHint())
        grid.addWidget(btn,1,8)

        grid.setHorizontalSpacing(0)
        grid.setVerticalSpacing(0)
        #self.move(300, 150)
    #    self.setMaximumSize(450,450)
     #   self.setMinimumSize(450,450)
        self.setWindowTitle('Checkers')
        self.show()
        
def main():
    app = QtGui.QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
