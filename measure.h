#ifndef MEASURE_H
#define MEASURE_H
#define SIZE_PATTERN 24
#define SIZE 65
#include<hidapi.h>
#include"dmd.h"
struct DMD{
	hid_device *handle;
	struct Patterns * pattern;
	int szPattern;
};
void initDMD(struct DMD *dmd);
void moveDMD(const struct DMD dmd);
int talkDMD_char(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const char *data, const int sizeData);
int talkDMD_int(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const int *data, const int sizeData);
void closeDMD(struct DMD *dmd);

void reset(hid_device *handle);
void changeMode(hid_device *handle, int mode);
void stopSequence(hid_device *handle);
void startSequence(hid_device *handle);

void writeOnFIle(char *fileName, unsigned char *data, int size);
#endif

