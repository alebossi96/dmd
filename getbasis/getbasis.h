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

void getBasis(const int hadamard_raster, //1 to get hadamard basis 0 for raster scan
		const int dim, // in the case of hadamard it is the dimension of the hadamard basis, if it is raster scan it is the dimension of the line
		const int *idx, //index of basis or raster to have as output,
		const int szIdx,
		int ***output);

void getBasisHadamard(const int nBasis, const int *idx, const int szIdx, int ***basis);
void getBasisRaster(const int dim, const int *idx, const int szIdx, int ***basis);
#endif
