#define SIZE 65

#include"dmd.h"
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const int *data, const int &sizeData){
	char *tmp;
	tmp = (char *)malloc(sizeData*sizeof(char));
	for(int i = 0; i<sizeData; i++)
		tmp[i] = (char)data[i];

	int res = command(handle, mode, sequencebyte, com1,com2,tmp,sizeData);

	free(tmp);
	return res;
}


int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const char *data, const int &sizeData){
	FILE * pData = fopen("cData.txt", "a");	
	for(int k = 0; k<sizeData;k++){ 
		
		fprintf(pData, "%d\n", data[k]);
	}
	fclose(pData);
	FILE * pFile = fopen("cCommand.txt", "a");
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
	int *tmp = bitsToBytes(flagstring,8);
	buffer[1]=tmp[0];
	free(tmp);
	buffer[2]=sequencebyte;
	char *tmpChar;
	tmpChar = convlen(sizeData+2,16);
	tmp =bitsToBytes(tmpChar,16);
	buffer[3]=tmp[0];
	buffer[4]=tmp[1];
	free(tmp);
	free(tmpChar);
	buffer[5]= com2;
	buffer[6]=com1;
	long unsigned int tot = 7;
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
				fprintf(pFile, "%d\n", buffer[k]);
			}
			fprintf(pFile, "\n\n");
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
				fprintf(pFile, "%d\n", buffer[k]);
			}
			fprintf(pFile, "\n\n");
			printf("\n\n");
		}
		buffer[0] = 0x00;
		for(int i = 0; i<SIZE; i++) buffer[i]= 0;
		int i = 1;
		j = 58;
		while(j<sizeData){
			buffer[i] = data[j];
			

			j++;
			i++;
			if(i%65==0){
				
				printf("\n\n");
				if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE);
				printf("written bytes = %d \n", res);
				
				}else{
					for(int k = 0; k<SIZE;k++){ 
						printf("%d, ", buffer[k]);
						fprintf(pFile, "%d\n", buffer[k]);
					}
					fprintf(pFile, "\n\n");
					printf("\n\n");
				}
			
				i =1;
				
			}
		}
		if(i%65 !=0 && i != 1){
			while(i%65!=0){
				
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
					fprintf(pFile, "%d\n", buffer[k]);
				}
				fprintf(pFile, "\n\n");
				printf("\n\n");
			}
		}
	}
	/*if(!DEBUG)
		checkForErrors(handle);*/
	
	fclose(pFile);
	return 0;	
}


int command_(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const char *data, const int &sizeData){
	FILE * pFile = fopen("cCommand.txt", "a");
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
	int *tmp = bitsToBytes(flagstring,8);
	buffer[1]=tmp[0];
	free(tmp);
	buffer[2]=sequencebyte;
	char *tmpChar;
	tmpChar = convlen(sizeData+2,16);
	tmp =bitsToBytes(tmpChar,16);
	buffer[3]=tmp[0];
	buffer[4]=tmp[1];
	free(tmp);
	free(tmpChar);
	buffer[5]= com2;
	buffer[6]=com1;
	long unsigned int tot = 7;
	int j = 0;
	if((tot+sizeData)<SIZE){
		for(int i = 0;i<sizeData;i++ ) buffer[tot+i] =data[i];
		for(int i = tot+sizeData; i<SIZE; i++) buffer[i] = 0x00;
		if(!DEBUG){
			int res = hid_read(handle, buffer,SIZE);
			printf("read = %d \n", res);
			
		}else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				fprintf(pFile, "%d\n", buffer[k]);
			}
			fprintf(pFile, "\n\n");
			printf("\n\n");
		}

	}else{
		for(int i = 0; i<SIZE-tot; i++) buffer[i+tot]= data[i];
		
		if(!DEBUG){
			int res = hid_read(handle, buffer,SIZE);
			printf("read = %d \n", res);
			
		}else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
				fprintf(pFile, "%d\n", buffer[k]);
			}
			fprintf(pFile, "\n\n");
			printf("\n\n");
		}
		buffer[0] = 0x00;
		for(int i = 0; i<SIZE; i++) buffer[i]= 0;
		int i = 1;
		j = 58;
		while(j<sizeData){
			buffer[i] = data[j];
			

			j++;
			i++;
			if(i%65==0){
				
				printf("\n\n");
				if(!DEBUG){
				int res = hid_read(handle, buffer,SIZE);
				printf("read = %d \n", res);
				
				}else{
					for(int k = 0; k<SIZE;k++){ 
						printf("%d, ", buffer[k]);
						fprintf(pFile, "%d\n", buffer[k]);
					}
					fprintf(pFile, "\n\n");
					printf("\n\n");
				}
			
				i =1;
				
			}
		}
		if(i%65 !=0 && i != 1){
			while(i%65!=0){
				
				buffer[i] =0x00;
				j++;
				i++;
				}			
			if(!DEBUG){
				int res = hid_read(handle, buffer,SIZE);
				printf("read = %d \n", res);
				
			}else{
				for(int k = 0; k<SIZE;k++){ 
					printf("%d, ", buffer[k]);
					fprintf(pFile, "%d\n", buffer[k]);
				}
				fprintf(pFile, "\n\n");
				printf("\n\n");
			}
		}
	}

	
	fclose(pFile);
	return 0;	
}

/*
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const char *data, const int &sizeData){
		
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
	int *tmp = bitsToBytes(flagstring,8);
	buffer[1]=tmp[0];
	free(tmp);
	buffer[2]=sequencebyte;
	char *tmpChar;
	tmpChar = convlen(sizeData+2,16);
	tmp =bitsToBytes(tmpChar,16);
	buffer[3]=tmp[0];
	buffer[4]=tmp[1];
	free(tmp);
	free(tmpChar);
	buffer[5]= com2;
	buffer[6]=com1;
	long unsigned int tot = 7;
	int cont = 0;
	for(int i = 0; i<sizeData; i++){
		buffer[(tot-1)%(SIZE-1)+1]=data[i];//è un char non so cosa fare
		if(tot%SIZE == 0 ){
			cont++;
			
			for(int k = 0; k< SIZE; k++) printf("%d, ", buffer[k]);
			printf("\n");
			if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE);
				printf("written bytes = %d \n", res);
				
				}
			buffer[0]=0x00;
			i--;
		}
		tot++;
	}
	while(tot%SIZE != 0) {
		buffer[tot%SIZE] = 0x00;
		tot++;
		}
	for(int k = 0; k< SIZE; k++) printf("%d, ", buffer[k]);
			printf("\n");
	if(!DEBUG){
		int res = hid_write(handle, buffer,SIZE);
		printf("written bytes = %d \n", res);
		
		}

		

	/*
	if((7+size_data)<=SIZE){
		int i;
		for(i =0; i<size_data;i++)
			buffer[7+i]=data[i]-'0';//è un char non so cosa fare
		for(i = i+7; i<SIZE; i++)
			buffer[i]=0x00;
		}
	*/
	
/*
	return 0;	
}
*/
void checkForErrors(hid_device *handle){
	if( hid_error(handle)==NULL)
		printf("errori");
		//cosa intende per string nella documentazione
	unsigned char message[1] = {1};
	int res = hid_read(handle, message,1);
	char * flag = convlen(res, 8);
	for(int i = 0; i<8; i++)
	printf("%c", flag[i]);
	printf("\n");
	//if(flag[2]== '1'){
		//printf("errori");
		char *a= NULL;
		command_(handle, 'r',0x22,0x01,0x00, a, 0);
		int error = hid_read(handle, message,1);
		printf("error = %d \n", error);
		command_(handle, 'r',0x22,0x01,0x01,a, 0);
		message[0] = 128;
		int response = hid_read(handle, message,1);
		printf("response = %d \n", response);

	//}

}



int pow2_i(const int &exp){
	int res=1;
	for(int i =0; i<exp; i++)
		res*=2;
	return res;
}
char * convlen(int a, int l){//a<2^l
	/*
    This function converts a number "a" into a bit string of
    length "l".
	*/
	char * res;
	res = (char*)malloc(l * sizeof(char));
	l--;//mi muovo all'ultima posizione
	while(a!=1 && l>-1){
		if(a%2==0)
			res[l]='0';
		else
			res[l]='1';
		a/=2;

		l--;
	}
	res[l]='1';
	while(l>0){
		l--;
		res[l]='0';

		}


	return res;
}

int * bitsToBytes(const char *a,const int &size){
	int *bits_int;
	bits_int = (int*)malloc(size * sizeof(int));	
	for(int i = 0; i< size; i++){
		bits_int[i]=a[i]-'0';
		}

	int *res;
	res=bitsToBytes(bits_int,size);

	free(bits_int);

	return res;
}



int * bitsToBytes(const int *a,const int &size){
	int size_bytes;
	if(size%8>0)
		size_bytes = size/8+1;
	else
		size_bytes = size/8;
	
	int * bytes;
	bytes = (int*)malloc(size_bytes * sizeof(int));
	int j=size-1;
	for(size_t i = 0; i<size_bytes; i++){
		bytes[i]=0;
		int cont = 0;
		while(j>-1 && cont<8){
			bytes[i] += pow2_i(cont)*(a[j]);
			cont++;
			j--;
		}
	}
	return bytes;		
}
void changeMode(hid_device *handle, int mode_){
	char mode[1]={mode_};
	command(handle, 'w', 0x00, 0x1a, 0x1b, mode,1);
	
}
void stopSequence(hid_device *handle){
	char mode[1]={0};
	command(handle, 'w', 0x00, 0x1a, 0x24, mode,1);
	
}


void reset(hid_device *handle){
	char mode[1]={2};
	command(handle, 'w', 0x00, 0x02, 0x00, mode,1);
	
}

void configureLut(hid_device *handle,int size, int rep){
	char *im =convlen(size,11); //ho size in bit lunghezza 11
	char *r=convlen(rep,32);
	char string[48];
	for(int i=0; i<48; i++){
		if(i<32)
			string[i]=r[i];
		else if(i<37)
			string[i]='0';
		else
			string[i]=im[i-37];
		
		}
	
	free(im);
	free(r);
	int *tmp =  bitsToBytes(string,48);

	command(handle, 'w',0x00,0x1a,0x31,tmp,6);
	
	free(tmp);
}
int pow_i(const int &b,const int &exp){
	int res=1;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}
void mergeImages(int ***images, int ***res){
	for(int i=0; i<24; i++){
		for(int j=0; j<1080; j++){
			for(int k=0; k<1920;k++){
				res[j][k][(2-i/8)]=0;	
			}		
		}
		
	}
	for(int i=0; i<24; i++){
		for(int j=0; j<1080; j++){
			for(int k=0; k<1920;k++){
				res[j][k][(2-i/8)]+=images[i][j][k]*pow_i(2,i%8);	
			}		
		}
	}

}
void definePatterns(hid_device *handle, const int &index,const int &exposure,const int &bitdepth, const char *color,const int &triggerIn,const int &darkTime,const int &triggerOut,const int &patInd,const int &bitPos){
	char payload[12];
	char * tmpChar = convlen(index,16);
	int *index_ = bitsToBytes(tmpChar,16);
	payload[0]=index_[0];
	payload[1]=index_[1];
	free(index_);
	//free(tmpChar);

	tmpChar = convlen(exposure,24);
	int *exposure_ = bitsToBytes(tmpChar,24);
	for(int i=0;i<3; i++)	payload[2+i] = exposure_[i];
	free(tmpChar);
	free(exposure_);
	char optionsByte[8];
	optionsByte[7] = '1';
	char *bitDepth = convlen(bitdepth-1,3);
	for(int i = 0; i<3; i++) optionsByte[4+i] = bitDepth[i];
	
	for(int i = 0; i<3; i++) optionsByte[1+i] = color[i];
	if(triggerIn) optionsByte[0]='1';
	else optionsByte[0]='0';

	int *tmp;
	tmp =bitsToBytes(optionsByte,8);
	payload[5]= tmp[0];
	free(tmp);
	tmp = bitsToBytes(convlen(darkTime,24),24);
	for(int i = 0; i<3; i++) payload[6+i] = tmp[i];
	free(tmp);
	if(triggerOut == 1)
		tmpChar = convlen(0,8);
	else 
		tmpChar = convlen(1,8);

	tmp = bitsToBytes(tmpChar,8);
	payload[9] = tmp[0];
	free(tmp);
	//free(tmpChar);
	char lastBits[16];
	char *patInd_;
	patInd_ = convlen(patInd,11);

	char *bitPos_;
	bitPos_ = convlen(bitPos,5);

	for(int i = 0; i<5; i++) lastBits[i] =bitPos_[i];
	for(int i = 0; i<11; i++) lastBits[i+5] =patInd_[i];

	tmp = bitsToBytes(lastBits,16);

	payload[10]= tmp[0];
	payload[11]=tmp[1];
	printf("\n\npayload definePatterns\n\n");
	for(int i = 0; i<12; i++) printf("%d, ", payload[i]);
	printf("\n\n\n");
	FILE * pFile = fopen("cdefinePatterns.txt", "a");
	for(int i = 0; i<12; i++)
		fprintf(pFile, "%d\n", payload[i]);

	fclose(pFile);
	command(handle, 'w',0x00,0x1a,0x34,payload,12);
	
}

void defSequence(hid_device *handle,int ***matrixes,int *exposure,int *trigger_in, int *dark_time, int *trigger_out, int repetition, const int &size){

	stopSequence(handle);
	struct List * encodedImagesList =NULL;
	struct Node * sizes=NULL;
	configureLut(handle,size,repetition);
	int sizeList=0;
	//devo passare 24 in 24 immagini a mergeImages
	/*if(size%24==0)
		int sizePkg= size/24;
	else
		int sizePkg= size/24+1;*/
	//int sizePkg = 0;
	int i = 0;
	//int imageData[24][1080][1920];
	int ***imageData;
	//createTensor(imageData,24,1080,1920);
	imageData = (int ***)malloc(24*sizeof(int**));
	for(int i = 0; i<24; i++){
		imageData[i] = (int **)malloc(1080*sizeof(int*));
		for(int j = 0; j<1080; j++ ){
			imageData[i][j] = (int*)malloc(1920* sizeof(int));
		}
	}
 	
	while(i<size || i%24!=0){
		//	sizePkg++;	

		if(i%24==0){
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=0;
					}		
				}
			}
		if(i<size){
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=matrixes[i][j][k];
					}		
				}
			}
		else{
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=0;
					}		
				}
		}
		i++;
		if(i%24 == 0){//int mergedImagesint[1080][1920][3]
			
			int ***mergedImagesint;	
			//createTensor(mergedImagesint,1080,1920,3);
			mergedImagesint = (int ***)malloc(1080*sizeof(int**));
			for(int i = 0; i<1080; i++){
				mergedImagesint[i] = (int **)malloc(1920*sizeof(int*));
				for(int j = 0; j<1920; j++ ){
					mergedImagesint[i][j] = (int*)malloc(3* sizeof(int));
				}
			}
/*
			FILE * pImgData = fopen("cMerged.txt", "a");
			
				for(int i = 0; i<24; i++){
					for(int j = 0; j<1080; j++){
						for(int k = 0; k<1920; k++)
						fprintf(pImgData, "%d\n", imageData[i][j][k]);
						
				}
			}
			fclose(pImgData);
*/
			mergeImages(imageData,mergedImagesint);//ci sono dei problemi
			struct Node *bitString=NULL;
			int bytecount=0;
			FILE * pFile = fopen("cMerged.txt", "a");
			
				for(int i = 0; i<1080; i++){
					for(int j = 0; j<1920; j++){
						for(int k = 0; k<3; k++)
						fprintf(pFile, "%d\n", mergedImagesint[i][j][k]);
						
				}
			}
			fclose(pFile);
			newEncode2(mergedImagesint, &bitString, bytecount);
			
			//e qui dovrò creare un array da bitString
			int *tmp;
			tmp =(int*)malloc(bytecount*sizeof(int));
			struct Node *next;
			int j = 0;

			while(bitString!=NULL){
				
				tmp[j]=bitString->data;
				next = bitString->next;
				free(bitString);
				bitString = next;
				j++;
			}

			int *encoded;
			encoded =(int*)malloc(bytecount*sizeof(int));
			j=bytecount;
			
		
			while(j>0){
				j--;
				encoded[bytecount-j-1]=tmp[j];
				
			}
			FILE * pFile2 = fopen("cNewEncode.txt", "a");
			
				for(int i = 0; i<bytecount; i++){
					
					fprintf(pFile2, "%d\n", encoded[i]);
						
				
				}
			fclose(pFile2);
			
			printf("\n\n\n bytecount = %d \n\n\n", bytecount);

			for(int i = 0; i < bytecount; i++) printf("encoded[%d] = %d,\n ",i, encoded[i]);
			printf("\n");
			//getchar();
			free(tmp);
			push(&encodedImagesList,encoded);
			push(&sizes,bytecount);
			sizeList++;
			char c111[3]={'1','1','1'};

			for(int j = (i/24-1)*24; j<(i) && j<size; j++){
				definePatterns(handle, j, exposure[j],1,c111,trigger_in[j],dark_time[j],trigger_out[j],(i-1)/24,j-(i/24-1)*24);	
				printf("i= %d j = %d  (i-1)/24 =%d j-(i/24-1)*24 = %d",i,j,(i-1)/24,j-(i/24-1)*24);			
				//getchar();
			}
			//NON SONO CERTO DEGLI ULTIMI DUE!
			
		}
	}
	int j = 0;
	while(encodedImagesList!=NULL){
		//il prosimo check devo farlo qui
		setBmp(handle, (i-1)/24-j,sizes->data);	
		//if(!DEBUG)
		//checkForErrors(handle);		
		printf("\n\n");
		bmpLoad(handle,encodedImagesList->data,sizes->data);
		//if(!DEBUG)
		//checkForErrors(handle);	
		encodedImagesList = encodedImagesList->next;
		sizes = sizes->next;
		j++;
	}
	//disallocare sizes e encodedImagesList
	

}
/*	
void defSequence(hid_device *handle,int matrixes[1][1080][1920],int *exposure,int *trigger_in, int *dark_time, int *trigger_out, int repetition, const int &size){
	stopSequence(handle);
	struct List * encodedImagesList =NULL;
	struct Node * sizes=NULL;
	configureLut(handle,size,repetition);
	int sizeList=0;
	//devo passare 24 in 24 immagini a mergeImages
	/*if(size%24==0)
		int sizePkg= size/24;
	else
		int sizePkg= size/24+1;*//*
	//int sizePkg = 0;
	int i = 0;
	//int imageData[24][1080][1920];
	int ***imageData;
	imageData = (int ***)malloc(24*sizeof(int**));
	for(int i = 0; i<24; i++){
		imageData[i] = (int **)malloc(1080*sizeof(int*));
		for(int j = 0; j<1080; j++ ){
			imageData[i][j] = (int*)malloc(1920* sizeof(int));
		}
	}
	while(i<size || i%24!=0){
		//	sizePkg++;	

		if(i%24==0){
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=0;
					}		
				}
			}
		if(i<size){
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=matrixes[i][j][k];
					}		
				}
			}
		else{
			for(int j=0; j<1080; j++){
				for(int k=0; k<1920; k++){
					imageData[i%24][j][k]=0;
					}		
				}
		}
		i++;
		if(i%24 == 0){
			int mergedImagesint[1080][1920][3];	
			mergeImages(imageData,mergedImagesint);
			struct Node *bitString=NULL;
			int bytecount=0;
			newEncode(mergedImagesint, bitString, bytecount);
			//e qui dovrò creare un array da bitString
			int *tmp;
			tmp =(int*)malloc(bytecount*sizeof(int));
			struct Node *next;
			int j = 0;
			while(bitString!=NULL){
				tmp[j]=bitString->data;
				next = bitString->next;
				free(bitString);
				bitString = next;
				j++;
			}

			int *encoded;
			encoded =(int*)malloc(bytecount*sizeof(int));
			while(j>0){
				encoded[bytecount-j-1]=tmp[j];
				j--;
			}
			free(tmp);
			push(encodedImagesList,encoded);
			push(sizes,bytecount);
			sizeList++;
			char c111[3];
			for(int j = 0; j<3; j++) c111[i]='1';
			for(int j = (i/24-1)*24+1; j<(i+1) && i<size; j++)
				definePatterns(handle, j, exposure[j],1,c111,trigger_in[i],dark_time[i],trigger_out[i],i/24,j-i);	
			

		}
	}
	//int i = sizeList-1;

	while(encodedImagesList!=NULL){//QUI DEVO FARLO AL CONTRARIO!
		setBmp(handle, i,sizes->data);	
		bmpLoad(handle,encodedImagesList->data,sizes->data);
		encodedImagesList = encodedImagesList->next;
		sizes = sizes->next;
	}
	
		

}
*/

void startSequence(hid_device *handle){
	char mode[1]={2};
	command(handle,'w',0x00,0x1a,0x24,mode,1);
        
}

void push(struct Node **head, int data){
	struct Node* newRef= (struct Node*)malloc(sizeof(struct Node));
	newRef->data =data;
	newRef->next =(*head);
	(*head) = newRef;

}
void push(struct List **head, int *data){
	struct List* newRef= (struct List*)malloc(sizeof(struct List));
	newRef->data =data;
	newRef->next =(*head);
	(*head) = newRef;
}
int isRowEqual(const int *a, const int *b){
	for(int i=0; i<3;i++)
		if(a[i]!=b[i]) return 0;
	return 1;
}

void setBmp(hid_device *handle,const int  &index,const int &size){
	int payload[6];
	char index_[16];
	char *tmp;
	printf("index =%d \n",index);
	//getchar();
	tmp = convlen(index,5);
	for(int i =0; i<11; i++) index_[i] = '0';
	for(int i = 0; i<5; i++) index_[i+11] = tmp[i];

	//free(tmp);
	int *tmp2;
	tmp2= bitsToBytes(index_,16);
	for(int i = 0; i<2; i++) payload[i]= tmp2[i];
	free(tmp2);
	int *total;

	tmp = convlen(size,32);
	total = bitsToBytes(tmp,32);
	free(tmp);
	for(int i = 0; i<4; i++) payload[i+2]= total[i];
	FILE * pFile = fopen("cSetBmp.txt", "a");
	for(int i = 0; i<6; i++)
		fprintf(pFile, "%d\n", payload[i]);

	fclose(pFile);
	command(handle, 'w',0x00,0x1a,0x2a,payload,6);
}
void bmpLoad(hid_device *handle, const int *image, const int &size){
	
	printf("\n");
	int packNum= size/504 +1;
	int cont = 0;
	for(int i = 0; i<packNum; i++){
		printf("\n%d\n", i);
		if(i%100 == 0) printf("%d di %d\n", i, packNum);
		int *payload;
		int bits;
		char *leng;
		if(i<packNum-1){
			leng = convlen(504,16);
			bits =504;	
		}else{
			leng= convlen(size%504,16);
			bits = size%504;
		}
		int *tmp;
		tmp = bitsToBytes(leng,16);
		payload= (int*)malloc((bits+2)*sizeof(int));
		for(int j = 0; j<2; j++) payload[j] = tmp[j];
		for(int j = 0; j<bits; j++) payload[j+2] = image[504*i+j];
		FILE * pFile = fopen("cBmpLoad.txt", "a");
		for(int i = 0; i< bits+2; i++)
			fprintf(pFile, "%d\n", payload[i]);

		fclose(pFile);
		command(handle, 'w',0x11,0x1a,0x2b, payload, bits+2);
		
	}

}

void newEncode2(int ***image, struct Node **bitString, int &byteCount){
	printf("image[0][0][0]=%d\n", image[0][0][0]);
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(1920,16);
	int *res= bitsToBytes(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(1080,16);
	res= bitsToBytes(tmpChar,16);
	free(tmpChar);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	//tmpChar = convlen(0,32);
	//res=bitsToBytes(tmpChar,32);
	//free(tmpChar);
	
	for(int i = 0; i<4; i++) push(bitString, 0x00);
	struct Node *link = *bitString;
	//free(res);
	for(int i = 0; i<8; i++) push(bitString, 0xff);
	for(int i = 0; i<5; i++) push(bitString, 0x00);
	push(bitString, 0x02);
	push(bitString, 0x01);
	for(int i = 0; i<21; i++) push(bitString, 0x00);
	int n=0;
	//fino a qui va bene
	//da qui bho
	for(int i = 0; i<1080; i++){
		for(int j = 0; j<1920; j++){
			if(i != 0){if( isRowEqual(image[i][j],image[i-1][j])){
				while(j<1920 &&isRowEqual(image[i][j],image[i-1][j])){
					n++;
					j++;}
				push(bitString, 0x00);
				push(bitString, 0x01);				
				byteCount+=2;
				if(n>=128){
					push(bitString, (n & 0x7f) | 0x80);
					push(bitString, n>>7);
					byteCount+=2;
				}else{
					push(bitString, n);					
					byteCount++;
				}
				n=0;
				continue;
			}}
			if(j < 1919){if( isRowEqual(image[i][j],image[i][j+1])){
				n++;
			}}
			else{
				if(n>=128){
					push(bitString, (n & 0x7f) | 0x80);
					push(bitString, n>>7);
					byteCount+=2;
				}else{
					push(bitString, n);					
					byteCount++;
				}//se quella dopo è diversa stampa
				push(bitString, image[i][j][0]);
				push(bitString, image[i][j][1]);
				push(bitString, image[i][j][2]);
				byteCount+=3;
				n = 0;
			}
		}
		push(bitString, 0x00);
		push(bitString, 0x00);
		byteCount+=2;
	}
	push(bitString, 0x00);
	push(bitString, 0x01);
	push(bitString, 0x00);
	byteCount+=3;	
	while(byteCount%4!=0){
		
		push(bitString, 0x00);
		byteCount++;	
	}
	int size = byteCount;
	int *total;
	
	tmpChar = convlen(size,32);
	total = bitsToBytes(tmpChar,32);
	
	for(int q = 0; q<4; q++){//per riempiere i primi 4 
		link->data=total[3-q];
		link = link->next;
	}
	free(tmpChar);
	free(total);

}





void newEncode(int ***image, struct Node **bitString, int &byteCount){
	printf("image[0][0][0]=%d\n", image[0][0][0]);
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(1920,16);
	int *res= bitsToBytes(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(1080,16);
	res= bitsToBytes(tmpChar,16);
	free(tmpChar);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	//tmpChar = convlen(0,32);
	//res=bitsToBytes(tmpChar,32);
	//free(tmpChar);
	
	for(int i = 0; i<4; i++) push(bitString, 0x00);
	struct Node *link = *bitString;
	//free(res);
	for(int i = 0; i<8; i++) push(bitString, 0xff);
	for(int i = 0; i<5; i++) push(bitString, 0x00);
	push(bitString, 0x02);
	push(bitString, 0x01);
	for(int i = 0; i<21; i++) push(bitString, 0x00);
	int n=0;
	//fino a qui va bene
	//da qui bho
	for(int i = 0; i<1080; i++){
		for(int j = 0; j<1920; j++){
			if(i>0){
				if(isRowEqual(image[i][j],image[i-1][j])){
					while(j<1920 &&isRowEqual(image[i][j],image[i-1][j])){
						n++;
						j++;}
					push(bitString, 0x00);
					push(bitString, 0x01);				
					byteCount+=2;
					if(n>=128){

						push(bitString, (n & 0x7f) | 0x80);
						push(bitString, n>>7);
						byteCount+=2;
					}else{
						push(bitString, n);					
						byteCount++;
					}
					n=0;
				}else{//if(isRowEqual(image[i][j],image[i-1][j]))
					if(j<1919){
						if(isRowEqual(image[i][j],image[i][j+1])){
							n++;
							while(j<1919 && isRowEqual(image[i][j],image[i][j+1])){
								n++;
								j++;							
							}
							if(n>=128){

								push(bitString, (n & 0x7f) | 0x80);
								push(bitString, n>>7);
								byteCount+=2;
							}else{
								push(bitString, n);					
								byteCount++;
							}
							push(bitString, image[i][j][0]);
							push(bitString, image[i][j][1]);
							push(bitString, image[i][j][2]);
							byteCount+=3;
							//j++;<-----------------------
							n=0;
						}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) || isRowEqual(image[i][j+1],image[i-1][j+1])){
							push(bitString, 0x01);
							byteCount++;
							push(bitString, image[i][j][0]);
							push(bitString, image[i][j][1]);
							push(bitString, image[i][j][2]);
							byteCount+=3;
							//j++;<----------------
							n=0;					
						}else{//se j<1919e le condizioni isRowEqual non si verificano
							push(bitString, 0x00);
							
							byteCount++;
							struct Node *toAppend=NULL;
							while(j<1919 && !isRowEqual(image[i][j],image[i][j+1])){
								n++;//tutti quelli che sono diversi
								push(&toAppend, image[i][j][0]);
								push(&toAppend, image[i][j][1]);
								push(&toAppend, image[i][j][2]);
								j++;
							}
							if(n>=128){

								push(bitString, (n & 0x7f) | 0x80);
								push(bitString, n>>7);
								byteCount+=2;
							}else{
								push(bitString, n);					
								byteCount++;
							}
							//aggiungere le cose contenute in toAppend
							//problema==sono aggiunti al contrario
							int *tmp;
							tmp =(int*)malloc(n*sizeof(n));
							struct Node *next;
							for(int k = 0; toAppend!=NULL; k++){
								tmp[k]=toAppend->data;
								next = toAppend->next;
								free(toAppend);
								toAppend = next;
							}
							for(int k =n; k>0; k++)
								push(bitString, tmp[k-1]);
							byteCount+=n;
							free(tmp);
							n=0;
						}
					
					}else{ //if(j==1919)
						push(bitString, 0x01);
						byteCount++;
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount+=3;
						j++;
						n=0;
					}
				}//else di if(isRowEqual(image[i][j],image[i-1][j]))
			}else{//if(i>0) quindi i == 0
				if(j<1919){
					printf("\n initial  j= %d \n",j);
					if(isRowEqual(image[i][j],image[i][j+1])){
						
						n++;
						while(j<1919 && isRowEqual(image[i][j],image[i][j+1])){
							//printf("\n j= %d  n = %d\n",j, n);
							
							n++;
							j++;							
						}
						if(n>=128){
							//printf("\n j= %d  n = %d\n",j, n);
							//getchar();
							push(bitString, (n & 0x7f) | 0x80);
							push(bitString, n>>7);
							byteCount+=2;
						}else{
							push(bitString, n);					
							byteCount++;
						}
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount+=3;
						//j++;<--------------------
						n=0;
					}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) /*|| isRowEqual(image[i][j+1],image[i-1][j+1])*/){						push(bitString, 0x01);
						byteCount++;
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount+=3;
						//j++;<------------------
						n=0;					
					}else{//se j<1919e le condizioni isRowEqual non si verificano
						push(bitString, 0x00);
						byteCount++;
						struct Node *toAppend=NULL;
						while(j<1919 && !isRowEqual(image[i][j],image[i][j+1])){
							n++;//aggiungo tutti quelli diversi
							push(&toAppend, image[i][j][0]);
							push(&toAppend, image[i][j][1]);
							push(&toAppend, image[i][j][2]);
							j++;
						}
						if(n>=128){

							push(bitString, (n & 0x7f) | 0x80);
							push(bitString, n>>7);
							byteCount+=2;
						}else{
							push(bitString, n);					
							byteCount++;
						}
						//aggiungere le cose contenute in toAppend
						//problema==sono aggiunti al contrario
						int *tmp;
						tmp =(int*)malloc(n*sizeof(n));
						struct Node *next;
						for(int k = 0; toAppend!=NULL; k++){
							tmp[k]=toAppend->data;
							next = toAppend->next;
							free(toAppend);
							toAppend = next;
						}
						for(int k =n; k>0; k--)
							push(bitString, tmp[k-1]);
						byteCount+=n;
						free(tmp);
						n=0;
					}
					
				}else{ //if(j==1919)
					push(bitString, 0x01);
					byteCount++;
					push(bitString, image[i][j][0]);
					push(bitString, image[i][j][1]);
					push(bitString, image[i][j][2]);
					byteCount+=3;
					//j++;<---------------
					n=0;
				}


			}
		}//while(j<SIZE_H)
		push(bitString, 0x00);
		push(bitString, 0x00);
		byteCount+=2;
	
	}//while(i<SIZE_W)	
	push(bitString, 0x00);
	push(bitString, 0x01);
	push(bitString, 0x00);
	byteCount+=3;	
	while(byteCount%4!=0){
		
		push(bitString, 0x00);
		byteCount++;	
	}
	int size = byteCount;
	int *total;
	
	tmpChar = convlen(size,32);
	total = bitsToBytes(tmpChar,32);
	
	for(int q = 0; q<4; q++){//per riempiere i primi 4 
		link->data=total[3-q];
		link = link->next;
	}
	free(tmpChar);
	free(total);

}//function



void hadamard(int **matrix, const int &nBasis){
	int logN = log(nBasis)/log(2);

	if( nBasis%pow_i(2,logN) != 0)
		return;

	matrix[0][0] = 1;
	for(int i = 0; i< logN; i++){
		int size = pow2_i(i);
		for(int j =0; j<size; j++){
			for(int k = 0; k<size; k++){
				//matrix[j][j] = matrix[j][j]
				matrix [j+size][k+size] = -matrix[j][k];
				matrix [j][k+size] = matrix[j][k];
				matrix [j+size][k] = matrix[j][k];
			}
		}
	}



}

void getBasis(const int &nBasis, const int &nMeas, int ***basis){
	int **H;

	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));


	hadamard(H, nBasis);


	//come inserire? nel senso binning? padding?/ transformazione?

	//int mult = 32/nBasis;//devo cercare la più vicina potenza del 2
	
	int logN = log(nBasis)/log(2);
	int mult=WIDTH/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2;
	for(int i = 0; i <nMeas; i++){
		for(int j = 0; j<WIDTH;j++){
			
			if(j>(idxZeros-1) && j<(WIDTH-idxZeros)){
				int el =(H[i][(j-idxZeros)/mult]+1)/2;
				 for(int k = 0; k<HEIGHT; k++)
					basis[i][k][j] = el;
			}
			else for(int k = 0; k<HEIGHT; k++) basis[i][k][j] = 0;	
			
		}
		
	}
}


