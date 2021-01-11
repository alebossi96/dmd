#ifndef MEASURE_H
#define MEASURE_H

#include<hidapi.h>
#include"dmd.h"

void initDMD(DMD &dmd);
void moveDMD(const DMD &dmd);
int talkDMD(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const char *data, const int &sizeData);
int talkDMD(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const int *data, const int &sizeData);
void closeDMD(DMD &dmd);


#endif

