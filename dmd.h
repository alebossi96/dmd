#ifndef DMD_H
#define DMD_H

#include "main.h"
#include "getbasis/getbasis.h"
#include "hidapi/hidapi.h"

struct Node{
	unsigned char data;
	struct Node *next;
};

struct List{
	int *data;
	struct List *next;
};

struct Patterns{
	unsigned char (*defPatterns)[12]; // pattern settings (exposure time, bit-depth, triggers, pattern index...)
	unsigned char configureLut[6]; // configuration of Look up table (LUT)
	unsigned char (*setBmp)[6]; // image settings (index, size)
	unsigned char ***bmpLoad; // compressed BMP image to load
	int nB;
	int *packNum; // number of data packets (max 504 bytes) to be loaded
	int nEl;
	int **bitsPackNum; // size of each packet
	int *exposure;
	int numOfBatches;
};


/* unused (all at the end of .c file) */
//int command(hid_device *handle, const char mode, const char sequencebyte, const char com2, const char com1, const int *data, const int sizeData);
//int newEncodeDMDScopeFoundary(int ***image, struct Node **bitString);
//void commandPattern(hid_device *handle, struct Patterns * pattern, const int szPattern);
//void checkForErrors(hid_device *handle);

/* write on .txt file (for debug when DMD is not connected) */
void writeOnFile(char *fileName, unsigned char *data, int size);

/* same of writeOnFile but with (int) data variable (used only in defSequence) */
void writeOnFile_int(char *fileName, int data [], int size);

/* This function converts a number "a" into a bit string of  length "l"
note: should be a < 2^l (it is not verified) */
char * convlen(int a, int l);

/* convert a string of bits in an array of char "a" in bytes (number 0 - 255)
returns the reference to the last byte converted
note: size is assumed to be multiple of 8, no controls are done
remember to free bytes after calling the function */
int * bitsToBytes(const char *a, const int size);

/* function to send data to the DMD (same to talk_DMD) UNUSED*/
//int command(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const char *data, const int sizeData);

/* configure the LUT for setting the sequence of patterns
p. 71 DLPC900 Programmer's guide (LUT configuration) */
void configureLut(struct Patterns * pattern, int size, int rep);

/* to process the image we need to compress it to a 3D 8bit array such that we can encode in each bit a pixel of an image,
we can store here 24 images.
It is the reason why I go on a step of 24 for uploading the images */
void mergeImages(unsigned char ***images, unsigned char ***res);

/* It defines the characteristic of a single pattern. (exposure time, bit-depth, triggers, pattern index...)
p.74 DLPC900 Programmer's guide */
void definePatterns(struct Patterns * pattern,const int index,const int exposure,const int bitdepth, const char *color,const int triggerIn,const int darkTime,const int triggerout,const int patInd,const int bitpos);

/* all the information useful for showing the patterns is produced here.
it requires only what pattern, the exposure and if triggers need to be presents */
void defSequence(struct Patterns * pattern, unsigned char ***matrixes, int *exposure, int *trigger_in, int dark_time, int *trigger_out, int repetition, const int size);

/* function to push data in LIFO structure */
void push(struct Node **head, unsigned char data);

/* given a number of elements "nEl" in a batch,
the function allocates memory for storing the pattern and save its addresses in struct Patterns*/
void allocatePattern(struct Patterns *p, int nEl);

/* function for eRLE to verify if planes of mergedImages are equal
return 0 if they are not equal
return 1 if they are equal */
int isRowEqual(const unsigned char *a, const unsigned char *b);

/* sets parameters (index and number of bytes) for image (group of patterns)
p. 75 DLPC900 Programmer's guide (initialize BMP Load) */
void setBmp(struct Patterns * pattern, const int  index, const int size);

/* load (or load to Patterns) the encoded image to the DMD
p. 75-76 DLPC900 Programmer's guide */
void bmpLoad(struct Patterns * pattern, const int index, const unsigned char *image, const int size);

/* This function encodes the images into something the dmd is able to read.
This function is the translation into C of the function "encode" present in https://github.com/csi-dcsc/Pycrafter6500
It uses a method called enhanced run lenght encoding described in the programming manual of the DMD6500/DMD9000
p.62 DLPC900 Programmer's Guide */
int newEncode2(unsigned char ***image, struct Node **n);

/* take data of the pattern from txt file */
int takeFromTxt(struct Node **bitString);

void readBMP(char* filename, int *** image);

/* basic mathematical functions */
//int pow2_i(const int exp);
//int pow_i(const int b, const int exp);    // these are in ordering.c
//int min(const int a, const int b);
int celing(const int a, const int b);

#endif

