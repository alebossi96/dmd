#ifndef DMD_H
#define DMD_H
#define DEBUG false
#define SIZE_PATTERN 24
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include <stdbool.h> 
#include <string.h>
#include<hidapi.h>//probabilmente devo cancellarla dopo
#ifdef _WIN32
	#include<windows.h>
#endif
	
#include"getbasis/getbasis.h"



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
	int nSetPerSet;
	int configureLut[6];
	int (*setBmp)[6];
	int ***bmpLoad;
	int nB;
	int *packNum;
	int nEl;
	int **bitsPackNum;
	int *exposure;

};


//int command(hid_device *handle, const char mode, const char sequencebyte, const char com2, const char com1, const char *data, const int sizeData);
//int command(hid_device *handle, const char mode, const char sequencebyte, const char com2, const char com1, const int *data, const int sizeData);
void commandPattern(hid_device *handle,struct Patterns * pattern, const int szPattern);
void checkForErrors(hid_device *handle);
int pow2_i(const int exp);
char * convlen(int a, int l);
int * bitsToBytes_char(const char *a, const int size);
int * bitsToBytes_int(const int *a, const int size);
void changeMode(hid_device *handle, int mode);
void stopSequence(hid_device *handle);
void configureLut(struct Patterns * pattern,int size, int rep);
void defSequence(struct Patterns * pattern,int ***matrixes,int *exposure,int *trigger_in, int dark_time, int *trigger_out, int repetition, const int size);
void reset(hid_device *handle);
void mergeImages(int ***images, int ***res);
int pow_i(const int b,const int exp);
int min(const int a,const int b);

int celing(const int a, const int b);
void allocatePattern(struct Patterns *p, int nB);



int isRowEqual(const int *a, const int *b);
void push(struct Node **head, int data);
int newEncode2(int ***image, struct Node **n);
void definePatterns(struct Patterns * pattern,const int index,const int exposure,const int bitdepth, const char *color,const int triggerIn,const int darkTime,const int triggerout,const int patInd,const int bitpos);
void setBmp(struct Patterns * pattern,const int index,const int size);
void bmpLoad(struct Patterns * pattern,const int index, const int *image, const int size);
void startSequence(hid_device *handle);

void readBMP(char* filename, int *** image);
#endif

