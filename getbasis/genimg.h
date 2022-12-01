#ifndef GENIMG_H
#define GENIMG_H
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include <stdbool.h> 
#include <string.h>
#include "genmat.h"
#define WIDTH 1920
#define HEIGHT 1080


void genImgHorLineFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis);
void genImgHorLineAdapt(const int **mat, int nBasis, int nMeas, int ***basis);
void genImgOblLineFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis);
void genImgOblLineAdapt(const int **mat, int nBasis, int nMeas, int ***basis);
void genImgHor2D(const int **mat, int nBasis, int nMeas, int xStart, int yStart, int dimX, int dimY, int xStop, int yStop, int ***basis);
void genImgObl2D(const int **mat, int nBasis, int nMeas, int ***basis);
void genImgHor2D(const int **mat, int nBasis, int nMeas, int ***basis);

int min(const int a, const int b);
#endif
