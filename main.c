#include"measure.h"
#define WIZARD_OR_COMMAND true
#define REMEMBER_PREVIUS_POS true

int main(){
	struct DMD dmd;
	struct Info info;
	info.previousPos = 0;
	while(true){
	
	if(WIZARD_OR_COMMAND){
		printf("if Raster scan press 0\n   Hadamard pattern press 1\n   all mirrors press 2\n   all closed press 3 \n   notch filter press 4\n  Hadamard Horiontal 5\n  Raster Horiontal 6 \n  Add One Line 7 Horizontal \n  Band Pass 8\n  Add One Line 9 Obliquo \n\n");
		scanf("%d", &(info.RasterOrHadamard));
		info.startPosition = 0;
		do{
			if(info.RasterOrHadamard == 1 || info.RasterOrHadamard== 0 || info.RasterOrHadamard== 5 || info.RasterOrHadamard== 6 || info.RasterOrHadamard== 7 || info.RasterOrHadamard== 9){
				if(info.RasterOrHadamard == 0 || info.RasterOrHadamard== 6 || info.RasterOrHadamard== 7|| info.RasterOrHadamard== 9 ){
					printf("\n\n RASTER \n\n");
					printf("select dimension of lines: ");
					scanf("%d",&(info.nBasis));
				}
				else{
					printf("\n\n HADAMARD \n\n");
					printf("select number of bases 	MUST BE POWER OF 2: ");
					scanf("%d", &(info.nBasis));
					info.startPosition = 0;
					//TODO mettere check su nBasis
				}
				printf("select number of measurement: ");
				scanf("%d",&(info.nMeas));
				printf("what should the size batch be? -must be multiple of 24. probably: " );
				scanf("%d",&(info.sizeBatch));
				
				
				printf("select exposure time in ms: ");
				scanf("%d", &(info.exp));
				info.exp *= 1000;
				printf("do you want to be repeated? if so press 1: ");
				scanf("%d", &(info.repeat));
				if(info.repeat !=1) info.repeat = 0;
				printf("do you want to be compressed? if so press 1: ");
				scanf("%d", &(info.compress));
				if(info.compress !=1) info.compress = 0;
						
			}
			else if(info.RasterOrHadamard == 2 ){
				printf("ALL ONES\n");
				info.nBasis =24; 
				info.nMeas = 24; 


				info.startPosition= 0;	
				
				
				info.exp = 500000;    

				info.repeat = 1;
				info.compress = 0;

			}
			else if(info.RasterOrHadamard == 3 ){
				printf("ALL ZEROS\n");
				info.nBasis =24; 
				info.nMeas = 24; 


				info.startPosition= 0;	
				
				
				info.exp = 500000;    

				info.repeat = 1;
				info.compress = 0;

			}
			else if(info.RasterOrHadamard == 4 || info.RasterOrHadamard == 8){
				printf("NOTCH FILTER");
				printf("select dimension of lines: ");
				scanf("%d",&(info.nBasis));
				printf("select start of rejection band:");
				scanf("%d",&(info.previousPos));
				info.nMeas = 24; 
				info.exp = 500000;
				info.repeat = 1;
				info.compress = 0;  

			}
			else{

				info.nBasis =24; 
				info.nMeas = 24; 


				info.startPosition= 0;	
				
				
				info.exp = 500000;    
				

				info.repeat = 1;
				info.compress = 0;


			}
		}while(!(info.RasterOrHadamard > -1 && info.RasterOrHadamard<10) 
		|| (info.RasterOrHadamard == 1 && info.nBasis<info.nMeas));
	}
	else{	info.RasterOrHadamard = 0;
		info.nBasis =300; 
		info.nMeas = 3; 
		info.startPosition= 0;	
		info.exp = 500000;    
		info.repeat = 1;
		info.compress = 0;
	}
	info.dark_time = 10000; 


	initDMD(info, &dmd);
	moveDMD(dmd);
	closeDMD(&dmd);
	if(info.RasterOrHadamard == 0 || info.RasterOrHadamard == 4){
		printf("select new starting position from 0 to 32: ");
		scanf("%d",&(info.startPosition));
		info.previousPos = offset(info.startPosition,info.nBasis, info.previousPos);//TODO deve essere aggiornato con nBasis precedente!
		printf("offset = %d\n", info.previousPos);
	}
	printf("press any key to continue. Press q to quit\n");
	//char a = getchar();

	//if(a == 'q' || a == 'Q') 
	return 0;
	
	}
	return 0;
}


