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
    grid=[]
    oldx=-1
    oldy=-1
    board = np.zeros(shape=(8,8))
    def img_name(self,i,j,piece):
        if(i%2==0 and j%2==0):
            if(piece==0):
                return "images/white.png"
            else:
                if(piece==9):
                    return "images/pawn_black_on_white.png"
                if(piece==-9):
                    return "images/pawn_white_on_white.png"
                if(piece==7):
                    return "images/castle_black_on_white.png"
                if(piece==-7):
                    return "images/castle_white_on_white.png"
                if(piece==5):
                    return "images/knight_black_on_white.png"
                if(piece==-5):
                    return "images/knight_white_on_white.png"
                if(piece==3):
                    return "images/bishop_black_on_white.png"
                if(piece==-3):
                    return "images/bishop_white_on_white.png"
                if(piece==2):
                    return "images/queen_black_on_white.png"
                if(piece==-2):
                    return "images/queen_white_on_white.png"
                if(piece==1):
                    return "images/king_black_on_white.png"
                if(piece==-1):
                    return "images/king_white_on_white.png"

        elif(i%2==1 and j%2==1):
            if(piece==0):
                return "images/white.png"
            else:
                if(piece==9):
                    return "images/pawn_black_on_white.png"
                if(piece==-9):
                    return "images/pawn_white_on_white.png"
                if(piece==7):
                    return "images/castle_black_on_white.png"
                if(piece==-7):
                    return "images/castle_white_on_white.png"
                if(piece==5):
                    return "images/knight_black_on_white.png"
                if(piece==-5):
                    return "images/knight_white_on_white.png"
                if(piece==3):
                    return "images/bishop_black_on_white.png"
                if(piece==-3):
                    return "images/bishop_white_on_white.png"
                if(piece==2):
                    return "images/queen_black_on_white.png"
                if(piece==-2):
                    return "images/queen_white_on_white.png"
                if(piece==1):
                    return "images/king_black_on_white.png"
                if(piece==-1):
                    return "images/king_white_on_white.png"

        else:
            if(piece==0):
                return "images/black.png"
            else:
                if(piece==9):
                    return "images/pawn_black_on_black.png"
                if(piece==-9):
                    return "images/pawn_white_on_black.png"
                if(piece==7):
                    return "images/castle_black_on_black.png"
                if(piece==-7):
                    return "images/castle_white_on_black.png"
                if(piece==5):
                    return "images/knight_black_on_black.png"
                if(piece==-5):
                    return "images/knight_white_on_black.png"
                if(piece==3):
                    return "images/bishop_black_on_black.png"
                if(piece==-3):
                    return "images/bishop_white_on_black.png"
                if(piece==2):
                    return "images/queen_black_on_black.png"
                if(piece==-2):
                    return "images/queen_white_on_black.png"
                if(piece==1):
                    return "images/king_black_on_black.png"
                if(piece==-1):
                    return "images/king_white_on_black.png"


    def on_click(self):
        #print("coming soon")
        command=["../alpha_beta"]
        for i in xrange(0,8):
            for j in xrange(7,-1,-1):
                command.append(str(self.board[j][i]))
                #print(self.board[j][i]),
           #print("")
        print(command)
        proc=subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        proc.stdin.write('q\n')
        out, err = proc.communicate()
        proc.wait()
        print(out)
        print("ended")

    def initBoard(self):
        board=self.board
        board[0][0]=7
        board[7][0]=-7
        board[0][1]=5
        board[7][1]=-5
        board[0][2]=3
        board[7][2]=-3
        board[0][3]=2
        board[7][3]=-2
        board[0][4]=1
        board[7][4]=-1
        board[0][5]=3
        board[7][5]=-3
        board[0][6]=5
        board[7][6]=-5
        board[0][7]=7
        board[7][7]=-7
        for i in xrange(0,8):
            board[1][i]=9
            board[6][i]=-9
        return board
    def mousePressEvent(self, QMouseEvent):

        posy=QMouseEvent.pos().x()
        posx=QMouseEvent.pos().y()
        btnx = 8 * posx / 400
        btny = 8 * posy / 400

        if(btnx>7 or btny>7):
            return

        w = QWidget()
        res=QMessageBox.question(w,"Action","Move or Remove","Move","Remove","Cancel")
        if(res==0):

            if(self.oldx!=-1 and self.oldy!=-1):


                self.board[btnx][btny]=self.board[self.oldx][self.oldy]
                self.board[self.oldx][self.oldy]=0
                label = QLabel()
                pixmap=QPixmap(self.img_name(self.oldx,self.oldy,self.board[self.oldx][self.oldy]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,self.oldx,self.oldy)

                label = QLabel()
                pixmap=QPixmap(self.img_name(btnx,btny,self.board[btnx][btny]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,btnx,btny)
                self.oldx=-1
                self.oldy=-1
            else:
                self.oldx=btnx
                self.oldy=btny
        
        if(res==1):
            self.board[btnx][btny]=0
            label = QLabel()
            pixmap=QPixmap(self.img_name(btnx,btny,self.board[btnx][btny]))
            label.setPixmap(pixmap)
            label.show()
            self.grid.addWidget(label,btnx,btny)
            self.oldy=-1
            self.oldx=-1

        if(res==2):
            self.oldy=-1
            self.oldx=-1
    
    def __init__(self):
        super(ChessInterface, self).__init__()
        self.initUI()                               
    def initUI(self):
        board=self.initBoard()
        #print(board)
        self.grid=QtGui.QGridLayout()
        self.setLayout(self.grid)

        for i in xrange(0,8):
            for j in xrange(0,8):
                label = QLabel()
                pixmap=QPixmap(self.img_name(i,j,board[i][j]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,i,j)
    

        w = QWidget()
        btn=QPushButton('Quit!',w)
        btn.setToolTip('Click to Exit!')
        btn.clicked.connect(exit)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,0,8)
        btn=QPushButton('Tip!',w)
        btn.setToolTip('Click to get a hint!')
        btn.clicked.connect(self.on_click)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,1,8)

        self.grid.setHorizontalSpacing(0)
        self.grid.setVerticalSpacing(0)
        self.setWindowTitle("Chess")
        self.show()

# main and other python stuffs
def main():
    app = QtGui.QApplication(sys.argv)
    ex = ChessInterface()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
