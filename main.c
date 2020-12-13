#include"dmd.h"
int main(){
	int nBasis =32;
	int nMeas =3;
	int exp = 1000000;
	int nSet = celing(nMeas,24);
	
	hid_device *handle = hid_open(0x0451, 0xc900, NULL);
	sleep(2);
	if (!handle) {
		printf("unable to open device\n");
		handle = NULL;
		if(!DEBUG) return 1;
	}
	stopSequence(handle);
	changeMode(handle, 3);
	int *exposure;
	int dark_time = 0;
	int trigger_in = 0;
	int trigger_out = 1;
	int ***basis;
	struct Patterns * pattern;
	pattern = (Patterns *)malloc(nSet*sizeof(Patterns));
	for (int q = 0; q < nSet; q++){
		int nB = min(24, nMeas-q*24);
		allocatePattern(&(pattern[q]),nB);
		//allocate memory
		exposure = (int *) malloc(nB * sizeof(int));
		basis = (int***)malloc(nB*sizeof(int**));
		for(int i = 0; i<nB; i++) basis[i] = (int**)malloc(1080*sizeof(int*));
		for(int i = 0; i<nB; i++){
			for(int j = 0;j<1080; j++) basis[i][j]= (int*)malloc(1920*sizeof(int));
		}
		// insert data
		for (int i = 0; i<nB; i++) exposure[i] = exp;
		for (int k = 0; (k+q*24)<nMeas && k<24; k++){
			for(int i = 0; i<HEIGHT; i++){
				for(int j = 0; j<WIDTH; j++){
					if(abs(j -5*k)<100 ) basis[k][i][j]=1;
					else basis[k][i][j]=0;
				}	
			}
		}

		//getBasis(nBasis,nMeas, basis);
		printf("da qui inizia defSequence\n");
		//fill pattern		

		defSequence(handle,&(pattern[q]),basis,exposure,trigger_in,dark_time,trigger_out, nB,nB);

		//free memory
		for(int i = 0; i<nB; i++){
			for(int j = 0; j<1080; j++)free(basis[i][j]);
			free(basis[i]);
		}

		free(basis);
		free(exposure); 
		//fare free pattern
	}
	for (int q = 0; q < nSet; q++){
		for(int i = 0; i<pattern[q].nB; i++)
			free(pattern[q].defPatterns[i]);
		free(pattern[q].defPatterns);
	}
	free(pattern);
	hid_close(handle);
	return 0;
}


