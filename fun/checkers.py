from matplotlib import pyplot as plt
import cv2
import numpy as np
white=cv2.imread("white.png")
black=cv2.imread("black.png")
r_black=cv2.imread("r_black.png")
b_black=cv2.imread("b_black.png")

k=1
tabuleiro = np.zeros(shape=(8,8))
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




fig=plt.figure()
k=1
#plt.subplots_adjust(top=0.1)

#fig.axes.get_xaxis().set_visible(False)
#fig.axes.get_yaxis().set_visible(False)
#plt.axis("off")
for i in xrange(0,8):
    for j in xrange(0,8):
        ax=fig.add_subplot(8,8,k)
        ax.set_axis_off()
        ax.set_xticklabels(())
        if tabuleiro[i][j]==0:
            ax.imshow(white)
        if tabuleiro[i][j]==1:
            ax.imshow(black)
        if tabuleiro[i][j]==2:
            ax.imshow(r_black)
        if tabuleiro[i][j]==3:
            ax.imshow(b_black)
        k=k+1
plt.subplots_adjust(hspace = 0)
plt.subplots_adjust(wspace = 0)
#plt.subplots_adjust(hspace = .0000001)
#plt.subplots_adjust(wspace = .0000001)
plt.show()
