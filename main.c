
#include "main.h"
#include "measure.h"

int main(){

	struct DMD dmd;
	struct Info info;
	bool ver = 0; // for verifications

	info.previousPos = 0;
	info.dark_time = 10000; /* dark time [microsec] = dead time to better sync DMD and SPC measurement*/

	while(true){

	/* SETTING PARAMETERS */
    /* WIZARD procedure (the program asks every setting) */
	if(WIZARD_OR_COMMAND){
		do{
            printf("Choose mode:\n (0) Raster scan\n (1) Hadamard pattern\n (2) All mirrors\n (3) All closed\n (4) Notch filter\n (5) Hadamard Horizontal\n (6) Raster Horizontal\n (7) Add One Line (Horizontal)\n (8) Band Pass\n (9) Add One Line (Oblique)\n (10) Hadamard 2D\n (-1) Exit\n\n >>");
            scanf("%d", &(info.RasterOrHadamard));
            getchar();
            info.startPosition = 0;
			if(info.RasterOrHadamard==1 || info.RasterOrHadamard==0 || info.RasterOrHadamard==5 || info.RasterOrHadamard==6 || info.RasterOrHadamard==7 || info.RasterOrHadamard==9 || info.RasterOrHadamard==10){
				if(info.RasterOrHadamard==0 || info.RasterOrHadamard==6 || info.RasterOrHadamard==7 || info.RasterOrHadamard==9 ){
					printf("\n\n*** RASTER ***\n");
					printf("Select dimension of lines: ");
					scanf("%d", &(info.nBasis));
					printf("Select number of measurements: ");
                    scanf("%d", &(info.nMeas));
				}
				else{
                    printf("\n\n*** HADAMARD ***\n");
                    do{
                        printf("Select number of bases (MUST BE POWER OF 2): ");
                        scanf("%d", &(info.nBasis));
                        info.startPosition = 0;
                        for(int i=2; i<5000; i*=2){ // check if nBasis is a power of 2
                            if (i==info.nBasis){
                                ver = 1;
                                break;
                            }
                        }
                    } while(!ver);
                    info.nMeas = info.nBasis; // in Hadamard mode we have 1 measure for each basis
                    if(info.RasterOrHadamard == 10){
                        printf("Select zoom: ");
                        scanf("%d", &(info.zoom));
                        printf("Select X (center = 960): ");
                        scanf("%d", &(info.xC));
                        printf("Select Y (center = 540): ");
                        scanf("%d", &(info.yC));
                    }
				}
				if(BATCHES){
                    printf("What should the size batch be? - must be multiple of 24. Probably: " );
                	scanf("%d", &(info.sizeBatch));
                }
                else
                    info.sizeBatch = info.nMeas;
				printf("Select exposure time in ms: ");
				scanf("%d", &(info.exp));
				info.exp *= 1000; // time is in microseconds
				printf("Do you want to be repeated? if so press 1: ");
				scanf("%d", &(info.repeat));
				if(info.repeat != 1) info.repeat = 0;
				printf("Do you want to use compression? if so press 1: "); // compression reduces the active part of the DMD to N central pixels
				scanf("%d", &(info.compress));
				getchar();
				if(info.compress != 1) info.compress = 0;
				printf("\n");
			}
			else if(info.RasterOrHadamard == 2 ){
				printf("\n\n*** ALL ONES ***\n");
				info.nBasis = 24;
				info.nMeas = 24;
				info.sizeBatch = 24;
				info.startPosition = 0;
				info.exp = 500000;
				info.repeat = 1;
				info.compress = 0;
			}
			else if(info.RasterOrHadamard == 3 ){
				printf("\n\n*** ALL ZEROS ***\n");
				info.nBasis = 24;
				info.nMeas = 24;
				info.sizeBatch = 24;
				info.startPosition = 0;
				info.exp = 500000;
				info.repeat = 1;
				info.compress = 0;
			}
			else if(info.RasterOrHadamard == 4 || info.RasterOrHadamard == 8){
				printf("\n\n*** FILTER ***\n");
				printf("Select dimension of line (in pixels): ");
				scanf("%d", &(info.nBasis));
				printf("Select start of band:");  // number of line or wavelength?
				scanf("%d", &(info.previousPos));
				getchar();
				info.nMeas = 24;
				info.sizeBatch = 24;
				info.exp = 500000;
				info.repeat = 1;
				info.compress = 0;
			}
            else if(info.RasterOrHadamard == -1){
                return 0;
            }
			else{
                printf("\n*** NOT VALID COMMAND \n\n");
			}
		} while(!(info.RasterOrHadamard > -2 && info.RasterOrHadamard < 11));
	}

	/* COMMAND procedure (the measure runs with the settings here below) */
	else{
        info.RasterOrHadamard = 0;
		info.nBasis = 300;
		info.nMeas = 3;
		info.sizeBatch = 24;
		info.startPosition = 0;
		info.exp = 500000;
		info.repeat = 1;
		info.compress = 0;
	}

	/* START TASKS */
	initDMD(info, &dmd);
	moveDMD(dmd);
	closeDMD(&dmd);
	if(info.RasterOrHadamard == 0 || info.RasterOrHadamard == 4){
		printf("Select new starting position from 0 to 32: ");
		scanf("%d", &(info.startPosition));
		getchar();
        info.previousPos = offset(info.startPosition, info.nBasis, info.previousPos); // TODO deve essere aggiornato con nBasis precedente! Non lo fa gia?
		printf("offset = %d\n", info.previousPos);
		printf("\n");
	}
	}
	return 0;
}

