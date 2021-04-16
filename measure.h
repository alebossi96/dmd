#ifndef MEASURE_H
#define MEASURE_H
#include<hidapi.h>
#include"dmd.h"
struct DMD{
	hid_device *handle;
	struct Patterns * pattern;
	int szPattern;
	int repeat;
};
struct Info{
	int RasterOrHadamard;
	int nBasis;
	int nMeas;
	int startPosition;
	int previousPos;
	int exp;
	int dark_time;	
	int repeat;
	int compress;
	int sizeBatch;
};
void initDMD(struct Info info,struct DMD *dmd);
void moveDMD(const struct DMD dmd);
int talkDMD_char(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const char *data, const int sizeData);
int talkDMD_int(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const int *data, const int sizeData);
void closeDMD(struct DMD *dmd);

void reset(hid_device *handle);
void changeMode(hid_device *handle, int mode);
void stopSequence(hid_device *handle);
void startSequence(hid_device *handle);


int offset(const int startPosition, const int lineWidth, const int previousPos );//returns the offset for the raster

#endif

