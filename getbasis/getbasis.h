#ifndef GETBASIS_H
#define GETBASIS_H
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include <stdbool.h> 
#include <string.h>
#define WIDTH 1920
#define HEIGHT 1080

void getBasis(const int hadamard_raster, //1 to get hadamard basis 0 for raster scan 2 for onlyOnes(for allignment)
		const int dim, // in the case of hadamard it is the dimension of the hadamard basis, if it is raster scan it is the dimension of the line
		const int *idx, //index of basis or raster to have as output,
		const int szIdx,
		int compressImage,
		int ***output);
int **ordering(const int nBasis, const int *idx, const int szIdx);
void getBasisHadamard(const int nBasis, const int *idx, const int szIdx,int compressImage, int ***basis);
void getBasisHadamardHorizontal(const int nBasis, const int *idx, const int szIdx,int compressImage, int ***basis);
void getBasisRasterHorizontal(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis);
void getBasisAddOneLineHorizontal(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis);
void getBasisRaster(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis);
void getBasisOnes(const int sz, int ***basis);
void getBasisZeros(const int sz, int ***basis);
void getBasisNotchFilter(const int *idx,const int dim, int ***basis);
void getBasisBandPass(const int *idx,const int dim, int ***basis);
int ** getBasisHadamardFromTxt(int nBasis, const int *idx, const int szIdx);
int nDigit(int n);
int min(const int a, const int b);
#endif
