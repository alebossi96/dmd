#include"measure.h"



void initDMD(struct DMD *dmd){
	/*
	char key[10];
	int value;
		
	FILE *fh;

	fh = fopen("getVariables.txt", "r");
	int i = 0;
	while (i<1) {
		i++;
		fscanf(fh, "%s %d", key, &value);
		printf("var = %s val = %d ", key, value)
	}
	fclose(fh);
	*/
	int nBasis =128;
	int nMeas = 50;
	int addBlank = 0;
	int numberOfRepetition = 24;
	int RasterOrHadamard = 1;
	if(!(!RasterOrHadamard || nBasis>=nMeas)) {
		printf("nBasis must be larger tha n nMeas!\n");
		return;
		}

	int exp = 1000000;   //1s 
	int expBlank = 100000;
	int dark_time = 0; // possibili alternativa a add blank 
	int sizePattern;
	if(addBlank)
		sizePattern = SIZE_SET_PATTERN/2;
	else
		sizePattern = SIZE_SET_PATTERN;
	int nSet = celing(nMeas,sizePattern);
	printf("nSet = %d \n", nSet);
	dmd->szPattern=nSet;
	hid_init();
	dmd->handle = hid_open(0x0451, 0xc900, NULL);
	sleep(2);
	if (!dmd->handle) {
		printf("unable to open device\n");
		dmd->handle = NULL;
		if(!DEBUG) return;// 1;
	}
	stopSequence(dmd->handle);
	changeMode(dmd->handle, 3);
	int *exposure;
	
	int *trigger_in; // 0 per non dovere mettere il trigger 1 per doverlo avere
	int *trigger_out; //1 per ricever trigger in output 0 per non averlo
	int ***basis;
	dmd->pattern = (struct Patterns *)malloc(nSet*sizeof(struct Patterns));
	for (int q = 0; q < nSet; q++){
		int nEl = min(sizePattern, nMeas-q*sizePattern); //contiene# di immagini caricate
		if(addBlank) nEl*=2;
		allocatePattern(&(dmd->pattern[q]),nEl);
		//allocate memory
		exposure = (int *) malloc(nEl * sizeof(int));
		trigger_in = (int *) malloc(nEl * sizeof(int));
		trigger_out = (int *) malloc(nEl * sizeof(int));
		basis = (int***)malloc(nEl*sizeof(int**));
		for(int i = 0; i<nEl; i++) basis[i] = (int**)malloc(1080*sizeof(int*));
		for(int i = 0; i<nEl; i++){
			for(int j = 0;j<1080; j++) basis[i][j]= (int*)malloc(1920*sizeof(int));
		}
		// insert data into pattern
		for (int i = 0; i<nEl; i++){
			if(!addBlank || i %2 == 0){ //<--non certo
				exposure[i] = exp;
				trigger_out[i] = 1;
				}
			else{
				exposure[i] = expBlank;
				trigger_out[i] = 0;
				}
			trigger_in[i] = 0;			
			}
			
		int nB;//nB contiene le info su quante info vere ci sono, # di basi caricate
		if(nMeas>(q+1)*sizePattern)
			nB = sizePattern;
		else
			nB = nMeas-q*sizePattern;
		int *idx;
		idx =(int* )malloc((nB)*sizeof(int));
		for(int i = 0; i<nB; i++)
			idx[i]=q*sizePattern+i;
		printf("nB = %d  nEl = %d \n", nB, nEl);
		getBasis(RasterOrHadamard,nBasis,idx,nB,addBlank, basis);
		free(idx);
		for (int k = 0; k<nEl ; k++){
			for(int i = 0; i<WIDTH; i+=100)
				printf("%d ",basis[k][0][i]);
						
			printf("\n");
		}
		
		dmd->pattern[q].nB =nB;
		defSequence(&(dmd->pattern[q]),basis,exposure,trigger_in,dark_time,trigger_out, /*numberOfRepetition*/nEl,nEl);//il penultimo o 1 o nEl1
		for(int i = 0; i<nEl; i++){
			for(int j = 0; j<HEIGHT; j++)free(basis[i][j]);
			free(basis[i]);
		}

		free(basis);
		free(exposure); 
		free(trigger_out);
		free(trigger_in);
	}


}
void moveDMD(const struct DMD dmd){
	//I only need to pass the data that we got from defSequence
	//all the rest must be processed during initialization or garbage collection
	for(int i = 0; i<dmd.szPattern; i++){
		//stopSequence(handle); 
		int totExposure = 0;
		for(int j = 0; j<dmd.pattern[i].nEl; j++){//prima era nB
			totExposure +=dmd.pattern[i].exposure[j];
			talkDMD_char(dmd.handle,'w',0x00,0x1a,0x34,dmd.pattern[i].defPatterns[j],12);
			}
		//configureLut
		talkDMD_int(dmd.handle,'w',0x00,0x1a,0x31,dmd.pattern[i].configureLut,6);
		
		for(int j = 0; j<dmd.pattern[i].nSetPerSet; j++){
			//setBmp
			talkDMD_int(dmd.handle,'w',0x00,0x1a,0x2a,dmd.pattern[i].setBmp[j],6);
			//bmpLoad
			for(int k = 0; k<dmd.pattern[i].packNum[j]; k++){
				talkDMD_int(dmd.handle,'w',0x11,0x1a,0x2b,dmd.pattern[i].bmpLoad[j][k],dmd.pattern[i].bitsPackNum[j][k]);
			}
		}
		stopSequence(dmd.handle);//
		//getchar();
		startSequence(dmd.handle);
		float tDead = 0.5; //0.5 s of dead time
		printf("totExposure = %d\n",totExposure);
		float tSleep = totExposure/1e6-tDead;
		if(totExposure/1e6-tDead<0)
			tSleep = 0;
		sleep(tSleep);//need to wait for the pattern to finish
					//sleep must be in input a number >0.001
		//TODO:nel TRS quando ho finito di ricevere tutti i trigger
	}


}
int talkDMD_int(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const int *data, const int sizeData){
	char *tmp;
	tmp = (char *)malloc(sizeData*sizeof(char));
	for(int i = 0; i<sizeData; i++)
		tmp[i] = (char)data[i];

	int res = talkDMD_char(handle, mode, sequencebyte, com1,com2,tmp,sizeData);

	free(tmp);
	return res;
}




int talkDMD_char(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const char *data, const int sizeData){
	//if(DEBUG) FILE * pFile = fopen("cCommand.txt", "a");
	unsigned char buffer[SIZE];
	char flagstring[8];	
	if(mode == 'r')
		flagstring[0]='1';
	else
		flagstring[0]='0';
	flagstring[1]='1';
	for(int i=2; i<8;i++)
		flagstring[i] ='0';
	buffer[0]=0x0;
	int *tmp = bitsToBytes_char(flagstring,8);
	buffer[1]=tmp[0];
	free(tmp);
	buffer[2]=sequencebyte;
	char *tmpChar;
	tmpChar = convlen(sizeData+2,16);
	tmp =bitsToBytes_char(tmpChar,16);
	buffer[3]=tmp[0];
	buffer[4]=tmp[1];
	free(tmp);
	free(tmpChar);
	buffer[5]= com2;
	buffer[6]=com1;
	long int tot = 7;
	int j = 0;
	if((tot+sizeData)<SIZE){
		for(int i = 0;i<sizeData;i++ ) buffer[tot+i] =data[i];
		for(int i = tot+sizeData; i<SIZE; i++) buffer[i] = 0x00;
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE);
			printf("written bytes = %d \n", res);
			
			
		}/*else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				//fprintf(pFile, "%d\n", buffer[k]);
			}
			//fprintf(pFile, "\n\n");
			printf("\n\n");
		}*/

	}else{
		for(int i = 0; i<SIZE-tot; i++) buffer[i+tot]= data[i];
		
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE);
			printf("written bytes = %d \n", res);
			
			
		}/*else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				//fprintf(pFile, "%d\n", buffer[k]);
			}
			//fprintf(pFile, "\n\n");
			printf("\n\n");
		}*/
		buffer[0] = 0x00;
		for(int i = 0; i<SIZE; i++) buffer[i]= 0;
		int i = 1;
		j = SIZE-tot;
		while(j<sizeData){
			buffer[i] = data[j];
			

			j++;
			i++;
			if(i%SIZE==0){

				if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE);
				
				printf("written bytes = %d \n", res);
				
				}/*else{
					for(int k = 0; k<SIZE;k++){ 
						printf("%d, ", buffer[k]);
						//fprintf(pFile, "%d\n", buffer[k]);
					}
					//fprintf(pFile, "\n\n");
					printf("\n\n");
				}*/
			
				i =1;
				
			}
		}
		if(i%SIZE !=0 && i != 1){
			while(i%SIZE!=0){
				
				buffer[i] =0x00;
				j++;
				i++;
				}			
			if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE);
				
				printf("written bytes = %d \n", res);
				
			}/*else{
				for(int k = 0; k<SIZE;k++){ 
					printf("%d, ", buffer[k]);
					//fprintf(pFile, "%d\n", buffer[k]);
				}
				//fprintf(pFile, "\n\n");
				printf("\n\n");
			}*/
		}
	}
	/*if(!DEBUG)
		checkForErrors(handle);*/
	
	//fclose(pFile);
	return 0;	
}
void closeDMD(struct DMD* dmd){
	for (int q = 0; q < dmd->szPattern; q++){
		for(int i = 0; i<dmd->pattern[q].nEl; i++)
			free(dmd->pattern[q].defPatterns[i]);
		free(dmd->pattern[q].defPatterns);
		for(int j = 0; j<dmd->pattern[q].nSetPerSet; j++){
			for(int i = 0; i<dmd->pattern[q].packNum[j]; i++)
				free(dmd->pattern[q].bmpLoad[j][i]);
			
			free(dmd->pattern[q].bmpLoad[j]);
			free(dmd->pattern[q].bitsPackNum[j]);
		}
		free(dmd->pattern[q].setBmp);
		free(dmd->pattern[q].bmpLoad);
		free(dmd->pattern[q].bitsPackNum);
		free(dmd->pattern[q].exposure);
		free(dmd->pattern[q].packNum);
		
	}

	free(dmd->pattern);
	//reset(dmd->handle);//bho elimina porcate?
	//if(!DEBUG) getchar();
	hid_close(dmd->handle);
	hid_exit();



}


