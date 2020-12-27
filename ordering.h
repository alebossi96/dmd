#ifndef ORDERING_H
#define ORDERING_H
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include <stdbool.h> 
#include <string.h>
//REMEMBER TO USE THE SAME ALGORITHM IN THE RECONSTRUCTION AS IN THE GETBASIS!
//BETTER IF YOU USE SAME FUNCTION!
void cake_cutting(const int &nBasis, int **matrix);
int numberOfCakes(int ** matrix,const int &rows,const int &cols);
int isSafe(int **M, int row, int col, bool **visited, int ROW, int COL);
void DFS(int **M, int row, int col, bool **visited, int ROW, int COL);
int countIslands(int **M,int ROW, int COL);
#endif
