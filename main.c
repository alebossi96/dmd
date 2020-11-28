#include"dmd.h"
int main(){
hid_device *handle = hid_open(0x0451, 0xc900, NULL);


if (!handle) {
	printf("unable to open device\n");
	handle = NULL;
	if(!DEBUG) return 1;
	}
changeMode(handle, 3);
int nMeas =4;
int *exposure;
int *dark_time;
int *trigger_in;
int *trigger_out;
int ***images;

exposure = (int *) malloc(nMeas * sizeof(int));
dark_time = (int *) malloc(nMeas * sizeof(int));
trigger_in = (int *) malloc(nMeas * sizeof(int));
trigger_out = (int *) malloc(nMeas * sizeof(int));
for (int i = 0; i<nMeas; i++) exposure[i] = 1000000;
for (int i = 0; i<nMeas; i++) dark_time[i]= 0;
for (int i = 0; i<nMeas; i++) trigger_in[i] = 0;
for (int i = 0; i<nMeas; i++) trigger_out[i] = 1;
//int images[1][1080][1920];
/*
images = (int***)malloc(size *sizeof(int**));
	for(int i = 0; i<size; i++){
		images[i] = (int**)malloc(1080*sizeof(int*));
		for(int j = 0;j<1080; j++)
			images[i][j]= (int*)malloc(1920*sizeof(int));
	}

for(int i = 0; i<1080; i++){
	for(int j = 0; j<1920; j++){
		if(j<1000 && i <500) images[0][i][j]=0;
		else images[0][i][j]=1;
	}	
	
}*/

	
	int ***basis;
	basis = (int***)malloc(nMeas*sizeof(int**));
	for(int i = 0; i<nMeas; i++){
		basis[i] = (int**)malloc(HEIGHT*sizeof(int*));
		for(int j = 0;j<HEIGHT; j++)
			basis[i][j]= (int*)malloc(WIDTH*sizeof(int));
	}
	getBasis(8,nMeas, basis);
/*
	for(int i = 0;i<nMeas; i++){
		for(int j = 0; j<HEIGHT; j++){
			for(int k = 0; k<WIDTH; k++){
				printf("%d, ", basis[i][j][k]);
			}
			printf("\n");
		}
	}
*/
printf("da qui inizia defSequence\n");
defSequence(handle,basis,exposure,trigger_in,dark_time,trigger_out, 60,nMeas);
startSequence(handle);

return 0;
}


