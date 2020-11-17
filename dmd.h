#ifndef DMD_H
#define DEBUG true
#include<hidapi.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com2, const char &com1, const char *data, const size_t &sizeData);
void checkForErrors(hid_device *handle);
int pow2_i(const int &exp);
char * convlen(int a, size_t l);
int * bitsToBytes(const char *a, const int &size);
int * bitsToBytes(const int *a, const int &size);
void changeMode(hid_device *handle, int mode);
void stopSequence(hid_device *handle);
void configureLut(hid_device *handle,int size, int rep);
void defSequence(hid_device *handle,int matrixes[1][1080][1920],int *exposure,int *trigger_in, int *dark_time, int *trigger_out, int repetition, const int &size);

void defSequenceTest(hid_device *handle,int matrixes[1][1080][1920],int *exposure,int *trigger_in, int *dark_time, int *trigger_out, int repetition, const int &size);
void mergeImages(int ***images, int res[1080][1920][3]);
int pow_i(const int &b,const int &exp);
struct Node{
	int data;
	struct Node *next;
};
struct List{
	int *data;
	struct List *next;
};
int isRowEqual(const int *a, const int *b);
void push(struct Node *head, int data);
void push(struct List *head, int* data);
void newEncode(const int image[1080][1920][3], struct Node *n, int &bytecount);
void definePatterns(hid_device *handle, const int &index,const int &exposure,const int &bitdepth, const char *color,const int &triggerIn,const int &darkTime,const int &triggerout,const int &patInd,const int &bitpos);
void setBmp(hid_device *handle,const int  &index,const int &size);
void bmpLoad(hid_device *handle, const int *image, const int &size);
void startSequence(hid_device *handle);

#endif

