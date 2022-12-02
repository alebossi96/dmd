#ifndef MEASURE_H
#define MEASURE_H

#include "main.h"
#include "hidapi/hidapi.h"
#include "dmd.h"

/** This structure contains data of the patterns for the dmd */
struct DMD{
	hid_device *handle; // pointer to the hid device
	struct Patterns *pattern; // structure containing the pattern for the DMD (see in dmd.h)
	int szPattern;
	int repeat;
};

/** This structure contains the parameters to set the measurements (type of pattern and duration) */
struct Info{
	int RasterOrHadamard; // selected mode for the measure
	int nBasis;
	int nMeas;
	int startPosition;
	int previousPos;
	int exp;
	int dark_time;
	int repeat;
	int compress;
	int sizeBatch;
	int zoom;
	int xC;
	int yC;
};

/** returns # of initial line for successive raster scans
startPosition has to be the channel of the SPC board that we want as first in the next measure */
int offset(const int startPosition, const int lineWidth, const int previousPos );

/** given info on the measurement and the struct dmd,
initializes DMD, allocates memory for the patterns, saves the pattern on the allocated memory, defines the sequence of the patterns */
void initDMD(struct Info info, struct DMD *dmd);

/** given the struct dmd initialized by initDMD,
sets DMD for displaying the pattern, loads pattern on DMD, start the sequence of patterns */
void moveDMD(const struct DMD dmd);

/** given the struct dmd,
frees allocated memory on the computer, disconnects the DMD */
void closeDMD(struct DMD *dmd);

/** print the pattern on .txt files */
void writePatternsOnFile(const int nEl, unsigned char ***basis);

#endif

