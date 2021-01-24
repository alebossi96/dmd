import ctypes 
import os.path
from numpy.ctypeslib import ndpointer
import numpy as np
def getbasis(index,nBasis):
	dll_name = "libgetbasis.so"
	dllabspath = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + dll_name
	orderingLib = ctypes.CDLL(dllabspath)
	nMeas = len(index)
	idx = (ctypes.c_int * nMeas)(*range(nMeas));
	for i in range(nMeas):
		idx[i]=index[i];
	orderingLib.ordering.restype=ctypes.POINTER(ctypes.POINTER(ctypes.c_int*32)*3)
	res = orderingLib.ordering(nBasis, idx, nMeas);
	basis = np.zeros((nMeas,nBasis))
	i = 0
	for el_i in res.contents:
		j = 0
		for el_j in el_i.contents:
			basis[i][j]=el_j
			j+=1
		i+=1
	return basis
if __name__=="__main__":
	index=[1,2,3,4,5]
	print(getbasis(index,32))
