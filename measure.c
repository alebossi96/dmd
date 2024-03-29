#include"measure.h"
#define SIZE_SEND 65
#define WIZARD_OR_COMMAND true
int offset(const int startPosition, const int lineWidth, const int previousPos ){// #initial line
	return startPosition*lineWidth + previousPos;}
void initDMD(struct Info info, struct DMD *dmd){
	hid_init();
	dmd->handle = hid_open(0x0451, 0xc900, NULL);

/*	START setup data needed*/
	int RasterOrHadamard, nBasis, nMeas, startPosition, exp,dark_time, repeat,compress,sizeBatch, previousPos;
	RasterOrHadamard= info.RasterOrHadamard;
	nBasis = info.nBasis;
	nMeas = info.nMeas;
	startPosition = info.startPosition;
	exp = info.exp;
	dark_time = info.dark_time;
	repeat = info.repeat;
	compress = info.compress;
	sizeBatch = info.sizeBatch;
	previousPos = info.previousPos;

	int offset_ = offset(startPosition,nBasis, previousPos)/nBasis; 
	int nSet = celing(nMeas,sizeBatch);
	printf("nSet = %d \n", nSet);
	dmd->szPattern=nSet;
	dmd->repeat = repeat;
	
	sleep(2);
	if (!dmd->handle) {
		printf("unable to open device\n");
		dmd->handle = NULL;
		if(!DEBUG) return;// 1;
	}
	//TODO:stopSequence(dmd->handle);
	changeMode(dmd->handle, 3);
	
	int *exposure;
	
	int *trigger_in; // 0 per non dovere mettere il trigger 1 per doverlo avere
	int *trigger_out; //1 per ricever trigger in output 0 per non averlo
	int ***basis;
	dmd->pattern = (struct Patterns *)malloc(nSet*sizeof(struct Patterns));
	for (int q = 0; q < nSet; q++){
		int nEl = min(sizeBatch, nMeas-q*sizeBatch); //contiene# di immagini caricate
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
			exposure[i] = exp;
			trigger_out[i] = 1;
			trigger_in[i] = 0;			
			}
			
		int nB;//nB contiene le info su quante info vere ci sono, # di basi caricate
		if(nMeas>(q+1)*sizeBatch)
			nB = sizeBatch;
		else
			nB = nMeas-q*sizeBatch;
		int *idx;
		idx =(int* )malloc((nB)*sizeof(int));
		for(int i = 0; i<nB; i++)
			idx[i]=q*sizeBatch+i+offset_;
		getBasis(RasterOrHadamard,nBasis,idx,nB, compress,basis);
		free(idx);
		for (int k = 0; k<nEl ; k++){
			for(int i = 0; i<WIDTH; i+=100)
				printf("%d ",basis[k][0][i]);
						
			printf("\n");
		}

		
		dmd->pattern[q].nB =nB;
		int numberOfRepetition;
		if(repeat)
			numberOfRepetition = 0;
		else 	
			numberOfRepetition = nEl; // the number can be as large as 500!
		defSequence(&(dmd->pattern[q]),basis,exposure,trigger_in,dark_time,trigger_out, numberOfRepetition,nEl);//il penultimo o 1 o nEl1
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
		stopSequence(dmd.handle);
		printf("press ENTER to start pattern");
		getchar();
		getchar();
		int totExposure = 0;
		
		for(int j = 0; j<dmd.pattern[i].nEl; j++){
			totExposure +=dmd.pattern[i].exposure[j];
			//define the pattern
			talkDMD_char(dmd.handle,'w',0x00,0x1a,0x34,dmd.pattern[i].defPatterns[j],12);
			if(!DEBUG)
			checkForErrors(dmd.handle);
			
			}
		talkDMD_int(dmd.handle,'w',0x00,0x1a,0x31,dmd.pattern[i].configureLut,6);
		if(!DEBUG)
		checkForErrors(dmd.handle);
		//setBmp
		for(int k = dmd.pattern[i].numOfBatches-1; k>=0; k--){
			talkDMD_int(dmd.handle,'w',0x00,0x1a,0x2a,dmd.pattern[i].setBmp[k],6);
			if(!DEBUG)
			checkForErrors(dmd.handle);
			//bmpLoad
			for(int j = 0; j<dmd.pattern[i].packNum[k]; j++){

				talkDMD_int(dmd.handle,'w',0x11,0x1a,0x2b,dmd.pattern[i].bmpLoad[k][j],dmd.pattern[i].bitsPackNum[k][j]);
			}
			if(!DEBUG)
			checkForErrors(dmd.handle);
		}
		//getchar();
		startSequence(dmd.handle);
		int tDead = 0; //0.5 s of dead time
		int tSleep = totExposure/1e6-tDead +1;
		printf("totExposure= %d\n",totExposure);
		if(totExposure/1e6-tDead<0)
			tSleep = 0;
		
		if(dmd.repeat){
			printf("press ENTER to continue");
			 getchar();
		}
		else
			sleep(tSleep+1);//need to wait for the pattern to finish	
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
	
	unsigned char buffer[SIZE_SEND];
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
	
	if((tot+sizeData)<SIZE_SEND){
		for(int i = 0;i<sizeData;i++ ) buffer[tot+i] =data[i];
		for(int i = tot+sizeData; i<SIZE_SEND; i++) buffer[i] = 0x00;
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE_SEND);
			
			printf("written bytes = %d \n", res);
			
			
		}else{
			for(int k = 0; k<SIZE_SEND;k++){ 
				printf("%d, ", buffer[k]);
				
			}
			writeOnFile("cCommand.txt",buffer, SIZE_SEND);
			printf("\n\n");
		}

	}else{
		for(int i = 0; i<SIZE_SEND-tot; i++) buffer[i+tot]= data[i];
		
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE_SEND);
			printf("written bytes = %d \n", res);
			
			
		}else{
			for(int k = 0; k<SIZE_SEND;k++){ 
				printf("%d, ", buffer[k]);
				
			}
			writeOnFile("cCommand.txt",buffer, SIZE_SEND);
			printf("\n\n");
		}
		buffer[0] = 0x00;
		for(int i = 0; i<SIZE_SEND; i++) buffer[i]= 0;
		int i = 1;
		j = SIZE_SEND-tot;
		while(j<sizeData){
			buffer[i] = data[j];
			

			j++;
			i++;
			if(i%SIZE_SEND==0){

				if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE_SEND);
				
				printf("written bytes = %d \n", res);
				
				}else{
					for(int k = 0; k<SIZE_SEND;k++){ 
						printf("%d, ", buffer[k]);
						
					}
					writeOnFile("cCommand.txt",buffer, SIZE_SEND);
					printf("\n\n");
				}
			
				i =1;
				
			}
		}
		if(i%SIZE_SEND !=0 && i != 1){
			while(i%SIZE_SEND!=0){
				
				buffer[i] =0x00;
				j++;
				i++;
				}			
			if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE_SEND);
				
				printf("written bytes = %d \n", res);
				
			}else{
				for(int k = 0; k<SIZE_SEND;k++){ 
					printf("%d, ", buffer[k]);
					
				}
				writeOnFile("cCommand.txt",buffer, SIZE_SEND);
				printf("\n\n");
			}
		}
	}
	
	/*if(!DEBUG)
		checkForErrors(handle);*/
	
	//fclose(pFile);
	return 0;	
}
void closeDMD(struct DMD* dmd){
	for (int q = 0; q < dmd->szPattern; q++){
		free(dmd->pattern[q].defPatterns);
		for(int k = 0; k<dmd->pattern[q].numOfBatches; k++){
			for(int i = 0; i<dmd->pattern[q].packNum[k]; i++)
				free(dmd->pattern[q].bmpLoad[k][i]);
			free(dmd->pattern[q].bmpLoad[k]);
			free(dmd->pattern[q].bitsPackNum[k]);
			
		}
		free(dmd->pattern[q].bmpLoad);
		free(dmd->pattern[q].bitsPackNum);
		free(dmd->pattern[q].setBmp);
		free(dmd->pattern[q].packNum);
		free(dmd->pattern[q].exposure);
		
		
	}
	//reset(dmd->handle);
	free(dmd->pattern);
	hid_close(dmd->handle);
	hid_exit();



}


void changeMode(hid_device *handle, int mode_){
	char mode[1];
	mode[0]=mode_;
	talkDMD_char(handle, 'w', 0x00, 0x1a, 0x1b, mode,1);
	
}
void stopSequence(hid_device *handle){
	char mode[1]={0};
	talkDMD_char(handle, 'w', 0x00, 0x1a, 0x24, mode,1);
	
}

void startSequence(hid_device *handle){
	char mode[1]={2};
	talkDMD_char(handle,'w',0x00,0x1a,0x24,mode,1);
        
}
void reset(hid_device *handle){
	char mode[1]={2};
	talkDMD_char(handle, 'w', 0x00, 0x02, 0x00, mode,1);
	
}


