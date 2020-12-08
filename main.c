#include"dmd.h"
int main(){
hid_device *handle = hid_open(0x0451, 0xc900, NULL);
sleep(2);
//printf("reset");
//reset(handle);

if (!handle) {
	printf("unable to open device\n");
	handle = NULL;
	if(!DEBUG) return 1;
	}
stopSequence(handle);
changeMode(handle, 3);
int nMeas =400;
int nBasis =128;
int *exposure;
int *dark_time;
int *trigger_in;
int *trigger_out;
int ***images;

exposure = (int *) malloc(nMeas * sizeof(int));
dark_time = (int *) malloc(nMeas * sizeof(int));
trigger_in = (int *) malloc(nMeas * sizeof(int));
trigger_out = (int *) malloc(nMeas * sizeof(int));
for (int i = 0; i<nMeas; i++) exposure[i] = 100000;
for (int i = 0; i<nMeas; i++) dark_time[i]= 0;
for (int i = 0; i<nMeas; i++) trigger_in[i] = 0;
for (int i = 0; i<nMeas; i++) trigger_out[i] = 1;
//int images[1][1080][1920];
/*	
images = (int***)malloc(nMeas *sizeof(int**));
	for(int i = 0; i<nMeas; i++){
		images[i] = (int**)malloc(1080*sizeof(int*));
		for(int j = 0;j<1080; j++)
			images[i][j]= (int*)malloc(1920*sizeof(int));
	}
for (int k = 0; k<nMeas; k++){
	for(int i = 0; i<1080; i++){
		for(int j = 0; j<1920; j++){
			if(j<100*k ) images[k][i][j]=0;
			else images[k][i][j]=1;
		}	
		
	}
}

for(int i = 0; i<1080; i++){
	for(int j = 0; j<1920; j++){
		if(j<1000 && i <500) images[1][i][j]=1;
		else images[1][i][j]=0;
	}	
	
}

*/
	int ***basis;
	basis = (int***)malloc(nMeas*sizeof(int**));
	for(int i = 0; i<nMeas; i++){
		basis[i] = (int**)malloc(HEIGHT*sizeof(int*));
		for(int j = 0;j<HEIGHT; j++)
			basis[i][j]= (int*)malloc(WIDTH*sizeof(int));
	}

	//getBasis(nBasis,nMeas, basis);

	for (int k = 0; k<nMeas; k++){
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				//basis[k][i][j]=1;
				
				if(abs(j -5*k)<100 ) basis[k][i][j]=1;
				else basis[k][i][j]=0;
				
			}	
			
		}
	}



	for(int i = 0;i<nMeas; i++){
		for(int j = 0; j<HEIGHT; j+=200){
			for(int k = 0; k<WIDTH; k+= 200){
				printf("%d, ", basis[i][j][k]);
			}
			printf("\n");
		}
	}

//getchar();
			FILE * pFile = fopen("cImages.txt", "a");
			
				for(int i = 0; i<nMeas; i++){
					for(int j = 0; j<1080; j++){
						for(int k = 0; k<1920; k++)
						fprintf(pFile, "%d\n", basis[i][j][k]);
						
				}
			}
			fclose(pFile);



printf("da qui inizia defSequence\n");
//60 è #ripetizioni! potrebbe essere x quello che si ferma
defSequence(handle,basis,exposure,trigger_in,dark_time,trigger_out, nMeas,nMeas);
getchar();
startSequence(handle);
if(!DEBUG)
		checkForErrors(handle);


//stopSequence(handle);
return 0;
}


