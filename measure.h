#ifndef MEASURE_H
#define MEASURE_H

#include "main.h"
#include "hidapi/hidapi.h"
#include "dmd.h"

struct DMD{
	hid_device *handle; // pointer to the hid device
	struct Patterns *pattern; // structure containing the pattern for the DMD (see in dmd.h)
	int szPattern;
	int repeat;
};

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
};

/* returns # of initial line for successive raster scans
startPosition has to be the channel of the SPC board that we want as first in the next measure */
int offset(const int startPosition, const int lineWidth, const int previousPos );

/* given info on the measurement and the struct dmd,
initializes DMD, allocates memory for the patterns, saves the pattern on the allocated memory, defines the sequence of the patterns */
void initDMD(struct Info info, struct DMD *dmd);

/* given the struct dmd initialized by initDMD,
sets DMD for displaying the pattern, loads pattern on DMD, start the sequence of patterns */
void moveDMD(const struct DMD dmd);

/* given the struct dmd,
frees allocated memory on the computer, disconnects the DMD */
void closeDMD(struct DMD *dmd);

// FORSE LE PROSSIME FUNZIONI CONVIENE SPOSTARLE IN DMD.c e DMD.h

/* function to write data in the DMD buffer
for more informations look at the ti DLPC900 Programmer's Guide */
int talkDMD_char(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const unsigned char *data, const int sizeData);

/* same to talkDMD_char but data are in int format
a conversion to char is performed and (char)data are passed to talkDMD_char */
int talkDMD_int(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const int *data, const int sizeData);

/* changes the internal image processing function of the DMD
see DMD Programmer's Guide pag. 58 */
void changeMode(hid_device *handle, int mode);

/* function tells the DMD to stop the pattern display sequence (next start command restart from the beginning)
see DMD Programmer's Guide pag. 69 */
void stopSequence(hid_device *handle);

/* function tells the DMD to start reproducing the pattern sequence stored in memory
see DMD Programmer's Guide pag. 69 */
void startSequence(hid_device *handle);

/* performs a software reset of the DMD, USB connection must be reestablished before sending new file
see DMD Programmer's Guide pag. 32-33 */
void reset(hid_device *handle);

/* retrieve the error number code from the DMD
see DMD Programmer's Guide pag. 23 */
void checkForErrors(hid_device *handle);

/* retrieve the error message string from the DMD
see DMD Programmer's Guide pag. 23 */
void checkErrorMessage(hid_device *handle);

#endif

