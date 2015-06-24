#!/usr/bin/env python
# -*- coding: utf-8 -*-

#Disclaimer block

# Marcus Botacin - 103338
# Alexandre Brisighello - 101350
# Unicamp 2015 Guido
# Final Project - ChessPhi Interface

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
    depth=3 # default
    grid=[] # img grid
    oldx=-1 # old mouse positions (just for initialization)
    oldy=-1
    # init board = zeros
    board = np.zeros(shape=(8,8))

    #img name -> return figure path for given position
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

    #text handler
    def text_event(self,text):
        old=self.depth
        try:
            self.depth=int(text)
        except:
            self.depth=old

    # another player move
    def on_click3(self):
        # call alpha beta
        command=["../alpha_beta"]
        for i in xrange(0,8):
            for j in xrange(7,-1,-1):
                command.append(str(self.board[j][i]))
        command.append(str(1))
        command.append(str(self.depth))
        command.append(str(0))

        proc=subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        proc.stdin.write('q\n')
        out, err = proc.communicate()
        proc.wait()
        # Search for moviment string
        search_for="Mov : "
        index=out.find(search_for)
        # parse stdout long string
        xbase=int(out[index+7:index+8])
        ybase=int(out[index+11:index+12])
        xdest=int(out[index+17:index+18])
        ydest=int(out[index+20:index+21])
        # show widget information
        w = QWidget()
        QMessageBox.information(w,"Action","Move ("+str(xbase)+","+str(ybase)+") -> ("+str(xdest)+","+str(ydest)+")","Ok")



    def on_click4(self):
        #call alpha beta
        command=["../alpha_beta"]
        for i in xrange(0,8):
            for j in xrange(7,-1,-1):
                command.append(str(self.board[j][i]))
        command.append(str(1))
        command.append(str(self.depth))
        command.append(str(0))
        proc=subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        proc.stdin.write('q\n')
        out, err = proc.communicate()
        proc.wait()
        #search for last words
        search_for="Alpha Beta Finished"
        index=out.find(search_for)
        l=[]
        # get position by position
        for j in xrange(0,8):
            for i in xrange(0,8):
                l.append(out[index-5:index-2])
                index=index-4
            index=index-1
        #reverse order
        for i in xrange(0,8):
            for j in xrange(0,8):
                #update board, update grid view
                self.board[i][j]=l[63-(8*i)-j]
                label = QLabel()
                pixmap=QPixmap(self.img_name(i,j,self.board[i][j]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,i,j)

    # click on serial move
    def on_click2(self):
        #call alpha beta
        command=["../alpha_beta"]
        for i in xrange(0,8):
            for j in xrange(7,-1,-1):
                command.append(str(self.board[j][i]))
        command.append(str(-1))
        command.append(str(self.depth))
        command.append(str(0))
        proc=subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        proc.stdin.write('q\n')
        out, err = proc.communicate()
        proc.wait()
        #search for last words
        search_for="Alpha Beta Finished"
        index=out.find(search_for)
        l=[]
        # get position by position
        for j in xrange(0,8):
            for i in xrange(0,8):
                l.append(out[index-5:index-2])
                index=index-4
            index=index-1
        #reverse order
        for i in xrange(0,8):
            for j in xrange(0,8):
                #update board, update grid view
                self.board[i][j]=l[63-(8*i)-j]
                label = QLabel()
                pixmap=QPixmap(self.img_name(i,j,self.board[i][j]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,i,j)

    # serial tip click
    def on_click(self):
        # call alpha beta
        command=["../alpha_beta"]
        for i in xrange(0,8):
            for j in xrange(7,-1,-1):
                command.append(str(self.board[j][i]))
        command.append(str(-1))
        command.append(str(self.depth))
        command.append(str(0))

        proc=subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        proc.stdin.write('q\n')
        out, err = proc.communicate()
        proc.wait()
        # Search for moviment string
        search_for="Mov : "
        index=out.find(search_for)
        # parse stdout long string
        xbase=int(out[index+7:index+8])
        ybase=int(out[index+11:index+12])
        xdest=int(out[index+17:index+18])
        ydest=int(out[index+20:index+21])
        # show widget information
        w = QWidget()
        QMessageBox.information(w,"Action","Move ("+str(xbase)+","+str(ybase)+") -> ("+str(xdest)+","+str(ydest)+")","Ok")

    #init board -> always the same classical positioning
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

    # get grid position by estimating mouse click
    # faster than a handler by widget
    def mousePressEvent(self, QMouseEvent):

        posy=QMouseEvent.pos().x()
        posx=QMouseEvent.pos().y()
        btnx = 8 * posx / 400
        btny = 8 * posy / 400

        if(btnx>7 or btny>7):
            return

        w = QWidget()

        # On mouse click, what to do ?
        res=QMessageBox.question(w,"Action","Move or Remove","Move","Remove","Cancel")

        # Move
        #first click, store position
        #second click, put on new position and erase old position
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
        
        # remove, just clear
        if(res==1):
            self.board[btnx][btny]=0
            label = QLabel()
            pixmap=QPixmap(self.img_name(btnx,btny,self.board[btnx][btny]))
            label.setPixmap(pixmap)
            label.show()
            self.grid.addWidget(label,btnx,btny)
            self.oldy=-1
            self.oldx=-1

        #cancel, do nothing
        if(res==2):
            self.oldy=-1
            self.oldx=-1
    
    # init class and GUI
    def __init__(self):
        super(ChessInterface, self).__init__()
        self.initUI()                               
    def initUI(self):
        board=self.initBoard()
        #print(board)
        self.grid=QtGui.QGridLayout()
        self.setLayout(self.grid)

        #init board
        #for each i,j : get image name, set widget(label)
        for i in xrange(0,8):
            for j in xrange(0,8):
                label = QLabel()
                pixmap=QPixmap(self.img_name(i,j,board[i][j]))
                label.setPixmap(pixmap)
                label.show()
                self.grid.addWidget(label,i,j)
    
        #buttons and event handlers

        w = QWidget()
        btn=QPushButton('Quit!',w)
        btn.setToolTip('Click to Exit!')
        btn.clicked.connect(exit)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,0,8)
        btn=QPushButton('Serial Tip! (P1)',w)
        btn.setToolTip('Click to get a hint!')
        btn.clicked.connect(self.on_click)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,1,8)
        btn=QPushButton('Serial Move! (P1)',w)
        btn.setToolTip('Click to get a move!')
        btn.clicked.connect(self.on_click2)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,2,8)

        btn=QPushButton('Serial Tip! (P2)',w)
        btn.setToolTip('Click to get a hint!')
        btn.clicked.connect(self.on_click3)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,3,8)
        btn=QPushButton('Serial Move! (P2)',w)
        btn.setToolTip('Click to get a move!')
        btn.clicked.connect(self.on_click4)
        btn.resize(btn.sizeHint())
        self.grid.addWidget(btn,4,8)

        qle = QtGui.QLineEdit()
        qle.textChanged[str].connect(self.text_event)
        self.grid.addWidget(qle,5,8)

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
