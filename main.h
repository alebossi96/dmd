#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 65
#define SIZE_SEND 65
#define WIDTH 1920
#define HEIGHT 1080
#define SIZE_PATTERN 24             //
#define SIZE_BUFFER 2049

<<<<<<< HEAD
#define DEBUG true                 // if true, the program works when the device is disconnected. The bytes are written on .txt files
#define DMD_SIMULATOR true          // if true, create a series of .txt files with the patterns for the dmd
=======
#define DEBUG false                  // if true, the program works when the device is disconnected. The bytes are written on .txt files
>>>>>>> parent of 215cce4 (Added Hadamard 2D and DMD simulation with py code)
#define PRINT_BYTES false           // if DEBUG = true and PRINT_BYTES = true, print bytes on command window
#define WIZARD_OR_COMMAND true      // if true, the measure parameters are asked in the program
#define REMEMBER_PREVIUS_POS true
#define BATCHES false               // if false, only 1 batch is created (sizeBatch = nMeas)
                                    /*  the program allows the division of a single measure in separate batches
                                     *  the DMD can get up to 256 bases it is needed to separate in two batches if you want to use more than 256 bases
                                     *  it is no needed if you use max 256 bases
                                     */

#endif
