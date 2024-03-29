#define SIZE 65

#include"dmd.h"

void writeOnFile(char *fileName, unsigned char *data, int size){
	FILE * pData = fopen(fileName, "a");	
	for(int k = 0; k<size;k++){ 
		
		fprintf(pData, "%d\n", data[k]);
	}
	fprintf(pData, "\n\n");
	fclose(pData);
}

void writeOnFile_int(char *fileName, int data [], int size){

	FILE * pData = fopen(fileName, "a");	
	for(int k = 0; k<size;k++){ 
		fprintf(pData, "%d\n", data[k]);
	}
	fprintf(pData, "\n\n");
	fclose(pData);
}


void checkForErrors(hid_device *handle){
	/* it does not work. Probably not implemented correctly*/
	if( hid_error(handle)==NULL)
		printf("errori");
	//unsigned char message[1] = {0x0100};
	//int res = hid_read(handle, message,1);
	command(handle,'r',0x22,0x01,0x00,NULL,0);
	unsigned char message[1] = {0x81};
	int res =hid_read(handle, message,1);
	//char * flag = convlen(res, 8);
	
	//for(int i = 0; i<8; i++)
	printf("%d", res);
	//getchar();
	//printf("\n");
	//free(flag);
	/*
	//if(flag[2]== '1'){
		//printf("errori");
		char *a= NULL;
		command(handle, 'r',0x22,0x01,0x00, a, 0);
		int error = hid_read(handle, message,1);
		printf("error = %d \n", error);
		command(handle, 'r',0x22,0x01,0x01,a, 0);
		message[0] = 128;
		int response = hid_read(handle, message,1);
		printf("response = %d \n", response);

	//}
	*/
}

/*
command is the function that does the comunication with the dmd, every other function that wants to comunicate with the dmd need to use this function
There are two command function implemented because one could want to pass a vector of int and char
*/
/*
int command(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const int *data, const int sizeData){
	char *tmp;
	tmp = (char *)malloc(sizeData*sizeof(char));
	for(int i = 0; i<sizeData; i++)
		tmp[i] = (char)data[i];

	int res = command(handle, mode, sequencebyte, com1,com2,tmp,sizeData);

	free(tmp);
	return res;
}
*/



int command(hid_device *handle, const char mode, const char sequencebyte, const char com1, const char com2, const char *data, const int sizeData){
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

			}
			printf("\n");
		}

	}else{
		for(int i = 0; i<SIZE-tot; i++) buffer[i+tot]= data[i];
		
		if(!DEBUG){
			int res = hid_write(handle, buffer,SIZE);
			printf("written bytes = %d \n", res);
			
			
		}else{
			for(int k = 0; k<SIZE;k++){ 
				printf("%d, ", buffer[k]);
			}
			printf("\n");
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

					}

					printf("\n");
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
				}

				printf("\n");
			}
		}
	}
	/*if(!DEBUG)
		checkForErrors(handle);*/
	
	return 0;	
}


/*
We want to use command pattern if we want a fast comunication with the dmd.
all the data is processed before the execution.
This function only perform the upload to the dmd(via the command function)
*/
/*
void commandPattern(hid_device *handle,struct Patterns * pattern, const int szPattern){
	//I only need to pass the data that we got from defSequence
	//all the rest must be processed during initialization or garbage collection
	for(int i = 0; i<szPattern; i++){
		//stopSequence(handle); 
		int totExposure = 0;
		for(int j = 0; j<pattern[i].nEl; j++){
			totExposure +=pattern[i].exposure[j];
			command(handle,'w',0x00,0x1a,0x34,pattern[i].defPatterns[j],12);
			}
		//configureLut
		command(handle,'w',0x00,0x1a,0x31,pattern[i].configureLut,6);
		//setBmp
		command(handle,'w',0x00,0x1a,0x2a,pattern[i].setBmp,6);
		//bmpLoad
		for(int j = 0; j<pattern[i].packNum; j++){
			command(handle,'w',0x11,0x1a,0x2b,pattern[i].bmpLoad[j],pattern[i].bitsPackNum[j]);
		}
		stopSequence(handle);
		startSequence(handle);
		sleep(totExposure/1e6);//need to wait for the pattern to finish
					//sleep must be in input a number >0.001
	}

}*/
/*
void checkForErrors(hid_device *handle){
	/* it does not work. Probably not implemented correctly
	if( hid_error(handle)==NULL)
		printf("errori");
	unsigned char message[1] = {1};
	int res = hid_read(handle, message,1);
	char * flag = convlen(res, 8);
	for(int i = 0; i<8; i++)
	printf("%c", flag[i]);
	printf("\n");
	//if(flag[2]== '1'){
		//printf("errori");
		char *a= NULL;
		command(handle, 'r',0x22,0x01,0x00, a, 0);
		int error = hid_read(handle, message,1);
		printf("error = %d \n", error);
		command(handle, 'r',0x22,0x01,0x01,a, 0);
		message[0] = 128;
		int response = hid_read(handle, message,1);
		printf("response = %d \n", response);

	//}

}
*/


int pow2_i(const int exp){
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
	if(a== 0){
		for(; l>0; l--)
			res[l-1]='0';
	}
	else{
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

	}
	return res;
}
/* returns a vector of int and we start from the bits and we get the byte corresponding
*/

int * bitsToBytes_char(const char *a,const int size){
	int *bits_int;
	bits_int = (int*)malloc(size * sizeof(int));
		
	for(int i = 0; i< size; i++){
		bits_int[i]=a[i]-'0';
		}
	
	int *res;
	res=bitsToBytes_int(bits_int,size);

	free(bits_int);

	return res;
}



int * bitsToBytes_int(const int *a,const int size){
	int size_bytes;
	if(size%8>0)
		size_bytes = size/8+1;
	else
		size_bytes = size/8;
	
	int * bytes;
	bytes = (int*)malloc(size_bytes * sizeof(int));
	int j=size-1;
	for(int i = 0; i<size_bytes; i++){
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
void configureLut(struct Patterns * pattern,int size, int rep){
	
	
	char *im =convlen(size,11); 
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
	int *tmp =  bitsToBytes_char(string,48);
	for(int i = 0; i<6; i++) pattern->configureLut[i]= tmp[i];
	//command(handle, 'w',0x00,0x1a,0x31,tmp,6);
	
	free(tmp);
}
int pow_i(const int b,const int exp){
	int res=1;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}

/*to process the image we need to compress it to a 3D 8bit array such that we can encode in each bit a pixel of an image,
we can store here 24 images.
It is the reason why I go on a step of 24 for uploading the images
*/
void mergeImages(int ***images, int ***res){
	for(int i=0; i<SIZE_PATTERN; i++){
		for(int j=0; j<HEIGHT; j++){
			for(int k=0; k<WIDTH;k++){
				res[j][k][(2-i/8)]=0;	
			}		
		}
		
	}
	for(int i=0; i<SIZE_PATTERN; i++){
		for(int j=0; j<HEIGHT; j++){
			for(int k=0; k<WIDTH;k++){
				res[j][k][(2-i/8)]+=images[i][j][k]*pow_i(2,i%8);	
			}		
		}
	}

}

/* 
It defines the characteristic of a single pattern. most importantly its duration 

*/

void definePatterns(struct Patterns * pattern, const int index,const int exposure,const int bitdepth, const char *color,const int triggerIn,const int darkTime,const int triggerOut,const int patInd,const int bitPos){
	//char payload[12];
	unsigned char * payload;
	payload = (unsigned char *)malloc(12 *sizeof(unsigned char));
	char * tmpChar= NULL;
	tmpChar = convlen(index,16);
	
	int *index_ = bitsToBytes_char(tmpChar,16);
	payload[0]=index_[0];
	payload[1]=index_[1];
	free(index_);
	free(tmpChar);
	tmpChar = convlen(exposure,24);
	int *exposure_ = bitsToBytes_char(tmpChar,24);

	for(int i=0;i<3; i++)	payload[2+i] = exposure_[i];
	free(tmpChar);
	free(exposure_);
	char optionsByte[8];
	optionsByte[7] = '1';
	char *bitDepth = NULL;
	bitDepth = convlen(bitdepth-1,3);
	for(int i = 0; i<3; i++) optionsByte[4+i] = bitDepth[i];
	free(bitDepth);
	
	for(int i = 0; i<3; i++) optionsByte[1+i] = color[i];
	if(triggerIn) optionsByte[0]='1';
	else optionsByte[0]='0';
	int *tmp;
	tmp =bitsToBytes_char(optionsByte,8);
	payload[5]= tmp[0];
	free(tmp);
	tmpChar = convlen(darkTime,24);
	tmp = bitsToBytes_char(tmpChar,24);
	free(tmpChar);
	for(int i = 0; i<3; i++) payload[6+i] = tmp[i];
	free(tmp);
	if(triggerOut==1)
	tmpChar = convlen(0,8);
	else
	tmpChar = convlen(1,8);
	tmp = bitsToBytes_char(tmpChar,8);
	payload[9] = tmp[0];
	free(tmp);
	free(tmpChar);
	char lastBits[16];
	char *patInd_;
	patInd_ = convlen(patInd,11);

	char *bitPos_;
	bitPos_ = convlen(bitPos,5);

	for(int i = 0; i<5; i++) lastBits[i] =bitPos_[i];
	for(int i = 0; i<11; i++) lastBits[i+5] =patInd_[i];

	tmp = bitsToBytes_char(lastBits,16);

	payload[10]= tmp[0];
	payload[11]=tmp[1];
	for(int i = 0; i<12; i++)
		pattern->defPatterns[bitPos + SIZE_PATTERN*patInd][i] = payload[i];
	writeOnFile("cPattern.txt",payload, 12);
	//command(handle, 'w',0x00,0x1a,0x34,payload,12);
	free(payload);
	free(tmp);
	free(bitPos_);
	free(patInd_);
	
	
}

/*
all the information useful for showing the patterns is produced here.
it requires only what pattern, the exposure and if triggers need to be presents
*/

void defSequence(struct Patterns * pattern,int ***matrixes,int *exposure,int *trigger_in, int dark_time, int *trigger_out, int repetition, const int size){
	int *encoded;
	int i = 0;
	int ***imageData;
	int szEncoded;
	imageData = (int ***)malloc(SIZE_PATTERN*sizeof(int**));
	for(int i = 0; i<SIZE_PATTERN; i++){
		imageData[i] = (int **)malloc(HEIGHT*sizeof(int*));
		for(int j = 0; j<HEIGHT; j++ ){
			imageData[i][j] = (int*)malloc(WIDTH* sizeof(int));
		}
	}
	int ***mergedImagesint;	

	mergedImagesint = (int ***)malloc(HEIGHT*sizeof(int**));
	for(int i = 0; i<HEIGHT; i++){
		mergedImagesint[i] = (int **)malloc(WIDTH*sizeof(int*));
		for(int j = 0; j<WIDTH; j++ ){			
			mergedImagesint[i][j] = (int*)malloc(3* sizeof(int));
		}
	}
	int numberOfBatch;
	if(size % SIZE_PATTERN == 0)
		numberOfBatch = size/SIZE_PATTERN;
	else numberOfBatch = size/SIZE_PATTERN + 1;
	pattern->numOfBatches = numberOfBatch;
	pattern->bmpLoad = (int ***)malloc(numberOfBatch * sizeof(int **));
	pattern->setBmp = (int (*)[6])malloc(numberOfBatch *sizeof(int[6]));
	pattern->packNum = (int *)malloc(numberOfBatch * sizeof(int *));
	pattern->bitsPackNum =(int **)malloc(numberOfBatch*sizeof(int*));
	pattern->exposure=(int*)malloc(size*sizeof(int));

	while(i<size || i%SIZE_PATTERN!=0){

		/*if(i%SIZE_PATTERN==0){
			for(int j=0; j<HEIGHT; j++){
				for(int k=0; k<WIDTH; k++){
					imageData[i%SIZE_PATTERN][j][k]=0;
					}		
				}
			}*/
		if(i<size){
			for(int j=0; j<HEIGHT; j++){
				for(int k=0; k<WIDTH; k++){
					imageData[i%SIZE_PATTERN][j][k]=matrixes[i][j][k];
					}		
				}
			}
		else{
			for(int j=0; j<HEIGHT; j++){
				for(int k=0; k<WIDTH; k++){
					imageData[i%SIZE_PATTERN][j][k]=0;
					}		
				}
		}
		i++;
		if(i%SIZE_PATTERN == 0){
			

	

			struct Node *bitString=NULL;
			int bytecount=0;
			mergeImages(imageData,mergedImagesint);
			bytecount = newEncode2(mergedImagesint, &bitString);
			//bytecount = newEncodeDMDScopeFoundary(mergedImagesint, &bitString);
			//bytecount = takeFromTxt(&bitString);

			int *tmp;
			printf("bytecount = %d \n",bytecount);
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
			
			encoded =(int*)malloc(bytecount*sizeof(int));
			j=bytecount;
			szEncoded = bytecount;
			while(j>0){
				j--;
				
				encoded[bytecount-j-1]=tmp[j];
			}
			printf("szEncoded = %d \n", szEncoded);
			getchar();
			writeOnFile_int("cNewEncode.txt", encoded, szEncoded);
			free(tmp);
			char c111[3]={'1','1','1'};
			for(int j = (i/SIZE_PATTERN-1)*SIZE_PATTERN; j<i && j<size; j++){
				pattern->exposure[j]=exposure[j];
				printf("j = %d \n", j);
			
				//define pattern deve essere diviso perchè si indica di quale batch fa parte quello specifico pattern
				definePatterns( pattern, j, exposure[j],1,c111,trigger_in[j],dark_time,trigger_out[j],(i-1)/SIZE_PATTERN,j-(i/SIZE_PATTERN-1)*SIZE_PATTERN);	
			}

			int idx = (i-1)/SIZE_PATTERN;
			setBmp(pattern, idx,szEncoded);	
			bmpLoad(pattern,idx,encoded,szEncoded);
			free(encoded);
			
		}
	}
	
	
	
	

	configureLut(pattern,size,repetition);
	for(int i = 0; i<SIZE_PATTERN;i++){
		for(int j = 0; j<HEIGHT; j++) free(imageData[i][j]);
	}

	for(int i = 0; i<SIZE_PATTERN;i++){
		free(imageData[i]);
	}
	free(imageData);
	
	for(int i = 0; i<HEIGHT; i++){
		for(int j = 0; j<WIDTH; j++) free(mergedImagesint[i][j]);
	}

	for(int i = 0; i<HEIGHT; i++){
		free(mergedImagesint[i]);
	}
	free(mergedImagesint);
	
			
}




void push(struct Node **head, int data){
	struct Node* newRef= (struct Node*)malloc(sizeof(struct Node));
	newRef->data =data;
	newRef->next =(*head);
	(*head) = newRef;

}

void allocatePattern(struct Patterns *p, int nEl){
	p->nEl = nEl;
	p->defPatterns=(char (*)[12])malloc(nEl*sizeof(char [12]));
	
}
int isRowEqual(const int *a, const int *b){
	for(int i=0; i<3;i++)
		if(a[i]!=b[i]) return 0;
	return 1;
}

void setBmp(struct Patterns * pattern,const int  index,const int size){
	int payload[6];
	char index_[16];
	char *tmp;
	tmp = convlen(index,5);
	for(int i =0; i<11; i++) index_[i] = '0';
	for(int i = 0; i<5; i++) index_[i+11] = tmp[i];

	free(tmp);
	int *tmp2;
	tmp2= bitsToBytes_char(index_,16);
	for(int i = 0; i<2; i++) payload[i]= tmp2[i];
	free(tmp2);
	int *total;

	tmp = convlen(size,32);
	total = bitsToBytes_char(tmp,32);
	free(tmp);
	for(int i = 0; i<4; i++) payload[i+2]= total[i];
	free(total);
	for(int i = 0; i<6; i++) pattern->setBmp[index][i]=payload[i];
	writeOnFile_int("cSetBmp.txt",payload, 6);

}


/*
	load (or load to Patterns) the encoded image to the dmd

*/
void bmpLoad(struct Patterns * pattern,const int index, const int *image, const int size){

	int packNum= size/504 +1;
	pattern->bmpLoad[index]=(int **)malloc(packNum*sizeof(int*));
	pattern->packNum[index]=packNum;
	pattern->bitsPackNum[index] =(int *)malloc(packNum*sizeof(int));
	for(int i = 0; i<packNum; i++){
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
		
		pattern->bitsPackNum[index][i] = bits+2;
		
		int *tmp;
		tmp = bitsToBytes_char(leng,16);

		free(leng);
		payload= (int*)malloc((bits+2)*sizeof(int));
		for(int j = 0; j<2; j++) payload[j] = tmp[j];
		free(tmp);
		
		for(int j = 0; j<bits; j++) payload[j+2] = image[504*i+j];
		pattern->bmpLoad[index][i]=(int *)malloc((bits+2)*sizeof(int));
		for(int j = 0;j<bits+2;j++) pattern->bmpLoad[index][i][j]=payload[j];

		//command(handle, 'w',0x11,0x1a,0x2b, payload, bits+2);
		free(payload);
			
		
	}
	

}

/* 
this function encodes the images into something the dmd is able to read.

This function is the translation into C of the function "encode" present in https://github.com/csi-dcsc/Pycrafter6500

it uses a method called enhanced run lenght encoding described in the programming manual of the DMD6500/DMD9000
*/

int newEncode2(int ***image, struct Node **bitString){
	int byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(WIDTH,16);
	int *res= bitsToBytes_char(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(HEIGHT,16);
	res= bitsToBytes_char(tmpChar,16);
	free(tmpChar);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	for(int i = 0; i<4; i++) push(bitString, 0x00);
	struct Node *link = *bitString;

	for(int i = 0; i<8; i++) push(bitString, 0xff);
	for(int i = 0; i<5; i++) push(bitString, 0x00);
	push(bitString, 0x02);
	push(bitString, 0x01);
	for(int i = 0; i<21; i++) push(bitString, 0x00);


	//all of the above is just the header
	//below we have the actual encoding
	int n=0;
	int i = 0;
	int j = 0;

	while(i<HEIGHT){
		while(j<WIDTH){
			if(i>0 && isRowEqual(image[i][j],image[i-1][j])){
				for(;j<WIDTH&&isRowEqual(image[i][j],image[i-1][j]);j++,n++);
			push(bitString, 0x00);
			push(bitString, 0x01);
			byteCount+=2;
			if(n>=128){
				push(bitString, (n & 0x7f) | 0x80);
				push(bitString, n>>7);
				byteCount+=2;
			}else{
				push(bitString,n);
				byteCount++;
				}
			n=0;

			}else{
				if(j<1919 && isRowEqual(image[i][j],image[i][j+1])){
					n++;
					for(;j<WIDTH-1 && isRowEqual(image[i][j],image[i][j+1]); n++, j++);
					if(n>=128){
						push(bitString, (n & 0x7f) | 0x80);
						push(bitString, n>>7);
						byteCount+=2;
					}else{
						push(bitString,n);
						byteCount++;
					}
					push(bitString, image[i][j-1][0]);
					push(bitString, image[i][j-1][1]);
					push(bitString, image[i][j-1][2]);
					byteCount+=3;
					n=0;
					j++;
				}else{
					if(j>WIDTH-3 || 
						isRowEqual(image[i][j+1],image[i][j+2]) || 
						(i >0 && isRowEqual(image[i][j+1],image[i-1][j+1])) || 
						(i == 0 && isRowEqual(image[i][j+1],image[HEIGHT-1][j+1]))){

						push(bitString, 0x01);
						byteCount++;
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount +=3;
						j++;
						n = 0;
					}else{
						push(bitString, 0x00);
						byteCount++;
						struct Node *toAppend=NULL;
						for(; !isRowEqual(image[i][j],image[i][j+1]) &&( (i >0 && isRowEqual(image[i][j],image[i-1][j])) || (i == 0 && isRowEqual(image[i][j],image[HEIGHT-1][j]))) ; n++, j++){//cambiato qui
							push(&toAppend, image[i][j][0]);
							push(&toAppend, image[i][j][1]);
							push(&toAppend, image[i][j][2]);

						}
						if(n>=128){
							push(bitString, (n & 0x7f) | 0x80);
							push(bitString, n>>7);
							byteCount+=2;
						}else{
							push(bitString,n);
							byteCount++;
						}
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

					}
	

				}

			}
			j=0;
			i++;
			push(bitString,0x00);
			push(bitString,0x00);
			byteCount+=2;
	
		}	



	//}







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
	total = bitsToBytes_char(tmpChar,32);
	
	for(int q = 0; q<4; q++){//per riempiere i primi 4 
		link->data=total[3-q];
		link = link->next;
	}
	free(tmpChar);
	free(total);
	return byteCount;

}





int takeFromTxt(struct Node **bitString){
	FILE* file = fopen ("pyNewEncode.txt", "r");
 	int i = 0;
	int byteCount = 0;
  	fscanf (file, "%d", &i);    
  	while (!feof (file))
	{  byteCount++;
	      push(bitString, i);
	      fscanf (file, "%d", &i);
		printf("%d\n",i);      
	    }
	  fclose (file);
	return byteCount;     
}
int newEncodeDMDScopeFoundary(int ***image, struct Node **bitString){
	int byteCount;
	printf("image[0][0][0]=%d\n", image[0][0][0]);
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(1920,16);
	int *res= bitsToBytes_char(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(1080,16);
	res= bitsToBytes_char(tmpChar,16);
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
				printf("\n byteCount  = %d \n",byteCount);
					
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
							j++;//TODO<-----------------------
							n=0;
						}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) || isRowEqual(image[i][j+1],image[i-1][j+1])){
							push(bitString, 0x01);
							byteCount++;
							push(bitString, image[i][j][0]);
							push(bitString, image[i][j][1]);
							push(bitString, image[i][j][2]);
							byteCount+=3;
							j++;//TODO<----------------
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
					
					}else if(j==1919){ //if(j<1919)
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
						j++;//TODO<--------------------
						n=0;
					}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) ||
						(i >0 && isRowEqual(image[i][j+1],image[i-1][j+1])) ||
						(i == 0 && isRowEqual(image[i][j+1],image[HEIGHT-1][j+1]))){						push(bitString, 0x01);
						byteCount++;
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount+=3;
						j++;//TODO;<------------------
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
					
				}else if(j == 1919){ //if(j==1919)
					push(bitString, 0x01);
					byteCount++;
					push(bitString, image[i][j][0]);
					push(bitString, image[i][j][1]);
					push(bitString, image[i][j][2]);
					byteCount+=3;
					j++;//TODO<---------------
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
	total = bitsToBytes_char(tmpChar,32);
	
	for(int q = 0; q<4; q++){//per riempiere i primi 4 
		link->data=total[3-q];
		link = link->next;
	}
	free(tmpChar);
	free(total);
	return byteCount;

}










int min(const int a,const int b){
	if(a>b)	return b;
	else return a;

}
int celing(const int a, const int b){
	if(a%b == 0)
		return a/b;
	return a/b +1;
}







void readBMP(char* filename, int *** image)
{
	//https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
	//io uso 3 canali qui 
	//ma può essere solo b/n

    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f); 

    // extract image height and width from header
    int width = *(int*)&info[18];//devo fare assert che sia uguale a mio widthS
    int height = *(int*)&info[22];
    
    // allocate 3 bytes per pixel
    int size = 3 * width * height;
    unsigned char* data = (unsigned char*)malloc(size*sizeof(unsigned char));

    // read the rest of the data at once
    fread(data, sizeof(unsigned char), size, f); 
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            // prendo solo da un canale.(se è rgb mi interessa solo 1)
	    int idx = i/3;
	    if(data[i]>127) (*image)[idx/width][idx%width]=1;
	    else (*image)[idx/width][idx%width]=0;
    }

}

