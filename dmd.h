#ifndef DMD_H
#define DEBUG true
#define WIDTH 1920
#define HEIGHT 1080
#include<hidapi.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include <stdbool.h> 
#include <string.h> 
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com2, const char &com1, const char *data, const int &sizeData);
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com2, const char &com1, const int *data, const int &sizeData);
void checkForErrors(hid_device *handle);
int pow2_i(const int &exp);
char * convlen(int a, int l);
int * bitsToBytes(const char *a, const int &size);
int * bitsToBytes(const int *a, const int &size);
void changeMode(hid_device *handle, int mode);
void stopSequence(hid_device *handle);
void configureLut(hid_device *handle,struct Patterns * pattern,int size, int rep);
void defSequence(hid_device *handle,struct Patterns * pattern,int ***matrixes,int *exposure,int trigger_in, int dark_time, int trigger_out, int repetition, const int &size);
void reset(hid_device *handle);
void mergeImages(int ***images, int ***res);
int pow_i(const int &b,const int &exp);
int min(const int &a,const int &b);
struct Node{
	int data;
	struct Node *next;
};
struct List{
	int *data;
	struct List *next;
};
struct Patterns{
	//need an array not matrix because Patterns will be array
	char **defPatterns;
	int configureLut[6];
	int setBmp[6];
	int **bmpLoad;
	int nB;
	int packNum;
	int *bitsPackNum;
	//to link
	struct Patterns *next;
	struct Patterns *previous;

};
int celing(const int &a, const int &b);
void allocatePattern(struct Patterns *p, int nB);



int isRowEqual(const int *a, const int *b);
void push(struct Node **head, int data);
void push(struct List **head, int* data);
void newEncode(int ***image, struct Node **n, int &bytecount);
void newEncode2(int ***image, struct Node **n, int &bytecount);
void newEncodeSimpleRLE(int ***image, struct Node **n, int &bytecount);
void definePatterns(hid_device *handle,struct Patterns * pattern,const int &index,const int &exposure,const int &bitdepth, const char *color,const int &triggerIn,const int &darkTime,const int &triggerout,const int &patInd,const int &bitpos);
void setBmp(hid_device *handle,struct Patterns * pattern,const int  &index,const int &size);
void bmpLoad(hid_device *handle,struct Patterns * pattern, const int *image, const int &size);
void startSequence(hid_device *handle);
void hadamard(int **matrix, const int &nBasis);
void getBasis(const int &nBasis, const int &nMeas, int ***Basis);
void cake_cutting(const int &nBasis, int **matrix);
int numberOfCakes(int ** matrix,const int &rows,const int &cols);

int isSafe(int **M, int row, int col, bool **visited, int ROW, int COL);
void DFS(int **M, int row, int col, bool **visited, int ROW, int COL);
int countIslands(int **M,int ROW, int COL);
 
#endif

