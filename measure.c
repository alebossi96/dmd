#include"measure.h"



void initDMD(struct DMD *dmd){
	
	int nBasis =32;
	int nMeas = 31;
	if(nBasis<nMeas) {
		printf("nBasis must be larger than nMeas!\n");
		return;
		}

	int exp = 1000000;   //1s 
	int expBlank = 1000; //1ms
	int addBlank = 1;
	int RasterOrHadamard = 1;
	int sizePattern;
	if(addBlank)
		sizePattern = SIZE_PATTERN/2;
	else
		sizePattern = SIZE_PATTERN;
	int nSet = celing(nMeas,sizePattern);
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
	int dark_time = 0;
	int trigger_in = 0;
	int trigger_out = 1;
	int ***basis;
	dmd->pattern = (struct Patterns *)malloc(nSet*sizeof(struct Patterns));
	for (int q = 0; q < nSet; q++){
		int nB = min(sizePattern, nMeas-q*sizePattern); //SBAGLIATO!
		if(addBlank) nB*=2;
		allocatePattern(&(dmd->pattern[q]),nB);
		//allocate memory
		exposure = (int *) malloc(nB * sizeof(int));//deve essere lunga solo nEl
		basis = (int***)malloc(nB*sizeof(int**));
		for(int i = 0; i<nB; i++) basis[i] = (int**)malloc(1080*sizeof(int*));
		for(int i = 0; i<nB; i++){
			for(int j = 0;j<1080; j++) basis[i][j]= (int*)malloc(1920*sizeof(int));
		}
		// insert data into pattern
		for (int i = 0; i<nB; i++){
			if(!addBlank || i %2 == 0) //<--non certo
			exposure[i] = exp;
			else
			exposure[i] = expBlank;
			}
		int nEl;//ma è la stessa roba di nb <----
		if(nMeas>(q+1)*sizePattern)
			nEl = sizePattern;
		else
			nEl = nMeas-q*sizePattern;
		int *idx;
		idx =(int* )malloc((nEl)*sizeof(int));
		for(int i = 0; i<nEl; i++)
			idx[i]=q*sizePattern+i;
		printf("nEl = %d \n", nEl);
		getBasis(RasterOrHadamard,nBasis,idx,nEl,addBlank, basis);
		free(idx);
		for (int k = 0; k<nEl ; k++){
			for(int i = 0; i<WIDTH; i+=100)
				printf("%d ",basis[k][0][i]);
						
			printf("\n");
		}
		
		dmd->pattern[q].nEl =nEl;
		defSequence(&(dmd->pattern[q]),basis,exposure,trigger_in,dark_time,trigger_out, nB,nB);//il penultimo o 1 o nEl
		for(int i = 0; i<nB; i++){
			for(int j = 0; j<HEIGHT; j++)free(basis[i][j]);
			free(basis[i]);
		}

		free(basis);
		free(exposure); 
	}


}
void moveDMD(const struct DMD dmd){
	//I only need to pass the data that we got from defSequence
	//all the rest must be processed during initialization or garbage collection
	for(int i = 0; i<dmd.szPattern; i++){
		//stopSequence(handle); 
		int totExposure = 0;
		for(int j = 0; j<dmd.pattern[i].nEl; j++){
			totExposure +=dmd.pattern[i].exposure[j];
			talkDMD_char(dmd.handle,'w',0x00,0x1a,0x34,dmd.pattern[i].defPatterns[j],12);
			}
		//configureLut
		talkDMD_int(dmd.handle,'w',0x00,0x1a,0x31,dmd.pattern[i].configureLut,6);
		//setBmp
		talkDMD_int(dmd.handle,'w',0x00,0x1a,0x2a,dmd.pattern[i].setBmp,6);
		//bmpLoad
		for(int j = 0; j<dmd.pattern[i].packNum; j++){
			talkDMD_int(dmd.handle,'w',0x11,0x1a,0x2b,dmd.pattern[i].bmpLoad[j],dmd.pattern[i].bitsPackNum[j]);
		}
		stopSequence(dmd.handle);//
		startSequence(dmd.handle);
		sleep(totExposure/1e6);//need to wait for the pattern to finish
					//sleep must be in input a number >0.001
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
			
			
		}else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				//fprintf(pFile, "%d\n", buffer[k]);
			}
			//fprintf(pFile, "\n\n");
			printf("\n\n");
		}

	}else{
		for(int i = 0; i<SIZE-tot; i++) buffer[i+tot]= data[i];
		
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE);
			printf("written bytes = %d \n", res);
			
			
		}else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				//fprintf(pFile, "%d\n", buffer[k]);
			}
			//fprintf(pFile, "\n\n");
			printf("\n\n");
		}
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
				
				}else{
					for(int k = 0; k<SIZE;k++){ 
						printf("%d, ", buffer[k]);
						//fprintf(pFile, "%d\n", buffer[k]);
					}
					//fprintf(pFile, "\n\n");
					printf("\n\n");
				}
			
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
				
			}else{
				for(int k = 0; k<SIZE;k++){ 
					printf("%d, ", buffer[k]);
					//fprintf(pFile, "%d\n", buffer[k]);
				}
				//fprintf(pFile, "\n\n");
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
		for(int i = 0; i<dmd->pattern[q].nB; i++)
			free(dmd->pattern[q].defPatterns[i]);
		free(dmd->pattern[q].defPatterns);
		for(int i = 0; i<dmd->pattern[q].packNum; i++)
			free(dmd->pattern[q].bmpLoad[i]);
	
		free(dmd->pattern[q].bmpLoad);
		free(dmd->pattern[q].exposure);
		free(dmd->pattern[q].bitsPackNum);
		
	}

	free(dmd->pattern);
	reset(dmd->handle);//bho elimina porcate?
	//if(!DEBUG) getchar();
	hid_close(dmd->handle);
	hid_exit();



}


