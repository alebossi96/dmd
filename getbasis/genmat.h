#ifndef GENMAT_H
#define GENMAT_H
#include<stdlib.h>
#include "ordering.h"
void genMatRast(int ** mat, int nLine,int nMeas,int offset);
void getMatHad(int ** mat,int nBasis, int nMeas, int ordering);
void genMatOne(int ** mat, int nLine,int nMeas);
void genMatZero(int ** mat, int nLine,int nMeas);
#endif
