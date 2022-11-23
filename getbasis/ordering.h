#ifndef ORDERING_H
#define ORDERING_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

//FOR THE RECONSTRUCTION OF THE MEASURE, REMEMBER TO USE THE SAME ALGORITHM USED IN GETBASIS!
//BETTER IF YOU USE SAME FUNCTION!

// QUESTE DUE FUNZIONI NON CI SONO !
//void cake_cutting(const int nBasis, int **matrix);
//int numberOfCakes(int ** matrix,const int rows,const int cols);

/* builds in matrix the Hadamard matrix of order nBasis
note: matrix must be preallocated and nBasis must be power of 2 (it is not verified!) */
void hadamard(short int **matrix, const int nBasis);

/* function to do DFS for a 2D boolean matrix. It only considers the 8 neighbours as adjacent vertices */
void DFS(short int **M, int row, int col, bool **visited, int ROW, int COL);

/* function that returns count of islands in a given boolean 2D matrix */
int countIslands(short int **M,int ROW, int COL);

/* returns 1 if:
row number is in range, column number is in range and site [row, col] is not yet visited */
int isSafe(short int **M, int row, int col, bool **visited, int ROW, int COL);

/* math functions */
int pow_i(const int b,const int exp);
int pow2_i(const int exp);
int min(const int a, const int b);

#endif
