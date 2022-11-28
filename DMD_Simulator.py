# -*- coding: utf-8 -*-
"""
CODE FOR SIMULATING THE DMD
(plot 2D .txt files)
put the file in the code folder
"""

import matplotlib.pyplot as plt
import numpy as np

nBasis = 16
path = "bin/Debug/"

fileName = []
for k in range(nBasis):
	if k<10:
		fileName.append("b00"+str(k)+".txt")
	elif k<100:
		fileName.append("b0"+str(k)+".txt")
	elif k<1000:
		fileName.append("b"+str(k)*".txt")

for k in range(nBasis):
	data = np.loadtxt(path+fileName[k], delimiter=",")
	plt.figure()
	plt.title("basis N "+str(k))
	plt.imshow(data)
	plt.show()
	
