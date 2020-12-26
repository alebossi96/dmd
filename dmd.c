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
		j = SIZE-tot;
		while(j<sizeData){
			buffer[i] = data[j];
			

			j++;
			i++;
			if(i%SIZE==0){
				
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

void commandPattern(hid_device *handle,struct Patterns * pattern, const int &szPattern){
	//solo quel che succede in defSequence devo comandare	
	for(int i = 0; i<szPattern; i++){
		//stopSequence(handle);ma qui
		//scrivo definePattern
		for(int j = 0; j<pattern[i].nEl; j++)
			command(handle,'w',0x00,0x1a,0x34,pattern[i].defPatterns[j],12);
		//configureLut
		command(handle,'w',0x00,0x1a,0x31,pattern[i].configureLut,6);
		//setBmp
		command(handle,'w',0x00,0x1a,0x2a,pattern[i].setBmp,6);
		//bmpLoad
		for(int j = 0; j<pattern[i].packNum; j++){
			command(handle,'w',0x11,0x1a,0x2b,pattern[i].bmpLoad[j],pattern[i].bitsPackNum[j]);
		}
		stopSequence(handle);//forse non va qui 
		startSequence(handle);
		//sleep(pattern[i].nEl);//aspetta che abbia finito
		sleep(1);
	}

}
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
		command(handle, 'r',0x22,0x01,0x00, a, 0);
		int error = hid_read(handle, message,1);
		printf("error = %d \n", error);
		command(handle, 'r',0x22,0x01,0x01,a, 0);
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

void configureLut(hid_device *handle,struct Patterns * pattern,int size, int rep){
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
	for(int i = 0; i<6; i++) pattern->configureLut[i]= tmp[i];
	command(handle, 'w',0x00,0x1a,0x31,tmp,6);
			FILE * pFile = fopen("cConfigureLut.txt", "a");
	for(int i = 0; i<6; i++)
		fprintf(pFile, "%d\n", tmp[i]);

	fclose(pFile);
	
	free(tmp);
}
int pow_i(const int &b,const int &exp){
	int res=1;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}
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
void definePatterns(hid_device *handle,struct Patterns * pattern, const int &index,const int &exposure,const int &bitdepth, const char *color,const int &triggerIn,const int &darkTime,const int &triggerOut,const int &patInd,const int &bitPos){
	char payload[12];
	char * tmpChar= NULL;
	tmpChar = convlen(index,16);
	
	int *index_ = bitsToBytes(tmpChar,16);
	payload[0]=index_[0];
	payload[1]=index_[1];
	free(index_);
	free(tmpChar);

	tmpChar = convlen(exposure,24);
	int *exposure_ = bitsToBytes(tmpChar,24);
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
	tmp =bitsToBytes(optionsByte,8);
	payload[5]= tmp[0];
	free(tmp);
	tmpChar = convlen(darkTime,24);
	tmp = bitsToBytes(tmpChar,24);
	free(tmpChar);
	for(int i = 0; i<3; i++) payload[6+i] = tmp[i];
	free(tmp);
	if(triggerOut==1)
	tmpChar = convlen(0,8);
	else
	tmpChar = convlen(1,8);
	tmp = bitsToBytes(tmpChar,8);
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

	tmp = bitsToBytes(lastBits,16);

	payload[10]= tmp[0];
	payload[11]=tmp[1];
	for(int i = 0; i<12; i++)
		pattern->defPatterns[bitPos][i] = payload[i];
	command(handle, 'w',0x00,0x1a,0x34,payload,12);
		FILE * pFile = fopen("cdefinePatterns.txt", "a");
	for(int i = 0; i<12; i++)
		fprintf(pFile, "%d\n", payload[i]);

	fclose(pFile);
	free(tmp);
	free(bitPos_);
	free(patInd_);
	
	
}

void defSequence(hid_device *handle,struct Patterns * pattern,int ***matrixes,int *exposure,int trigger_in, int dark_time, int trigger_out, int repetition, const int &size){
	int *encoded;
	stopSequence(handle);
	struct Node * sizes=NULL;
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
	
	while(i<size || i%SIZE_PATTERN!=0){
		//	sizePkg++;	

		if(i%SIZE_PATTERN==0){
			for(int j=0; j<HEIGHT; j++){
				for(int k=0; k<WIDTH; k++){
					imageData[i%SIZE_PATTERN][j][k]=0;
					}		
				}
			}
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
		if(i%SIZE_PATTERN == 0){//int mergedImagesint[HEIGHT][WIDTH][3]
			

	

			struct Node *bitString=NULL;
			int bytecount=0;
			mergeImages(imageData,mergedImagesint);
			newEncode2(mergedImagesint, &bitString, bytecount);
			

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
			
			encoded =(int*)malloc(bytecount*sizeof(int));
			j=bytecount;
			szEncoded = bytecount;
			while(j>0){
				j--;
				
				encoded[bytecount-j-1]=tmp[j];
			}
				
			printf("\n\n\n bytecount = %d \n\n\n", bytecount);

			
			free(tmp);
			//push(&encodedImagesList,encoded);
			//push(&sizes,bytecount);
			//sizeList++;
			char c111[3]={'1','1','1'};
			//free(encoded);//DA CANCELLARE
			
			for(int j = (i/SIZE_PATTERN-1)*SIZE_PATTERN; j<i && j<size; j++)
				definePatterns(handle, pattern, j, exposure[j],1,c111,trigger_in,dark_time,trigger_out,(i-1)/SIZE_PATTERN,j-(i/SIZE_PATTERN-1)*SIZE_PATTERN);	
			
			
		}
	}
	
	configureLut(handle,pattern,size,repetition);
	setBmp(handle, pattern, (i-1)/SIZE_PATTERN,szEncoded);	
	bmpLoad(handle,pattern,encoded,szEncoded);
	free(encoded);
	//encodedImagesList = encodedImagesList->next;
	//sizes = sizes->next;
	
	
	//disallocare sizes e encodedImagesList

	
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
void allocatePattern(struct Patterns *p, int nB){
	p->nB = nB;
	p->defPatterns=(char **)malloc(nB*sizeof(char *));
	for(int i = 0; i<nB; i++)
		p->defPatterns[i]=(char *)malloc(12*sizeof(char ));
}
int isRowEqual(const int *a, const int *b){
	for(int i=0; i<3;i++)
		if(a[i]!=b[i]) return 0;
	return 1;
}

void setBmp(hid_device *handle,struct Patterns * pattern,const int  &index,const int &size){
	int payload[6];
	char index_[16];
	char *tmp;
	printf("index =%d \n",index);
	//getchar();
	tmp = convlen(index,5);
	for(int i =0; i<11; i++) index_[i] = '0';
	for(int i = 0; i<5; i++) index_[i+11] = tmp[i];

	free(tmp);
	int *tmp2;
	tmp2= bitsToBytes(index_,16);
	for(int i = 0; i<2; i++) payload[i]= tmp2[i];
	free(tmp2);
	int *total;

	tmp = convlen(size,32);
	total = bitsToBytes(tmp,32);
	free(tmp);
	for(int i = 0; i<4; i++) payload[i+2]= total[i];
	free(total);
	for(int i = 0; i<6; i++) pattern->setBmp[i]=payload[i];
	FILE * pFile = fopen("cSetBmp.txt", "a");
	for(int i = 0; i<6; i++)
		fprintf(pFile, "%d\n", payload[i]);

	fclose(pFile);
	command(handle, 'w',0x00,0x1a,0x2a,payload,6);
}
void bmpLoad(hid_device *handle,struct Patterns * pattern,const int *image, const int &size){
	
	printf("\n");
	int packNum= size/504 +1;
	int cont = 0;
	pattern->bmpLoad=(int **)malloc(packNum*sizeof(int*));
	pattern->packNum=packNum;
	pattern->bitsPackNum =(int *)malloc(packNum*sizeof(int));
	for(int i = 0; i<packNum; i++){
		printf("\n%d\n", i);
		if(i%100 == 0) printf("%d di %d\n", i, packNum);
		int *payload;
		int bits;
		char *leng;
		printf("size = %d\n", size);
		if(i<packNum-1){
			leng = convlen(504,16);
			bits =504;	
		}else{
			leng= convlen(size%504,16);
			bits = size%504;
		}
		
		pattern->bitsPackNum[i] = bits+2;
		
		int *tmp;
		//stampa leng
		tmp = bitsToBytes(leng,16);

		free(leng);
		payload= (int*)malloc((bits+2)*sizeof(int));
		for(int j = 0; j<2; j++) payload[j] = tmp[j];
		free(tmp);
		
		for(int j = 0; j<bits; j++) payload[j+2] = image[504*i+j];
		pattern->bmpLoad[i]=(int *)malloc((bits+2)*sizeof(int));
		for(int j = 0;j<bits+2;j++) pattern->bmpLoad[i][j]=payload[j];
	
		command(handle, 'w',0x11,0x1a,0x2b, payload, bits+2);
		free(payload);
			
		
	}
	

}
void newEncodeSimpleRLE(int ***image, struct Node **bitString, int &byteCount){
	printf("image[0][0][0]=%d\n", image[0][0][0]);
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(HEIGHT,16);//inverto altezza con larghezza
	int *res= bitsToBytes(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(WIDTH,16);
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
	push(bitString, 0x01);//RLE compression
	push(bitString, 0x01);
	for(int i = 0; i<21; i++) push(bitString, 0x00);
	int n=0;
	for(int i = 0; i<WIDTH; i++){
		//sono tutte uguali le colonne!!CREDO
		for(int j = 0; j<HEIGHT/255; j+=1){
			if(j*255<HEIGHT) push(bitString, 255);
			else push(bitString,HEIGHT-(j-1)*255);
			push(bitString, image[0][i][0]);
			push(bitString, image[0][i][1]);
			push(bitString, image[0][i][2]);
			byteCount+=4;
		}
		push(bitString, 0x00);
		push(bitString, 0x00);
		byteCount+=2;
	}
	push(bitString, 0x00);
	push(bitString, 0x01);
	byteCount+=2;
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

void newEncode2(int ***image, struct Node **bitString, int &byteCount){
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(WIDTH,16);
	int *res= bitsToBytes(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(HEIGHT,16);
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
	total = bitsToBytes(tmpChar,32);
	
	for(int q = 0; q<4; q++){//per riempiere i primi 4 
		link->data=total[3-q];
		link = link->next;
	}
	free(tmpChar);
	free(total);

}





void newEncode(int ***image, struct Node **bitString, int &byteCount){
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	char*tmpChar;
	tmpChar = convlen(WIDTH,16);
	int *res= bitsToBytes(tmpChar,16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	free(tmpChar);
	tmpChar=convlen(HEIGHT,16);
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
	for(int i = 0; i<HEIGHT; i++){
		for(int j = 0; j<WIDTH; j++){
			if(i>0){
				if(isRowEqual(image[i][j],image[i-1][j])){
					while(j<WIDTH &&isRowEqual(image[i][j],image[i-1][j])){
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
					if(j<WIDTH-1){
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
						}else if(j>WIDTH-3 || isRowEqual(image[i][j+1],image[i][j+2]) || isRowEqual(image[i][j+1],image[i-1][j+1])){
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
							while(j<WIDTH-1 && !isRowEqual(image[i][j],image[i][j+1])){
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
				if(j<WIDTH-1){
					printf("\n initial  j= %d \n",j);
					if(isRowEqual(image[i][j],image[i][j+1])){
						
						n++;
						while(j<WIDTH-1 && isRowEqual(image[i][j],image[i][j+1])){
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
					}else if(j>WIDTH-3 || isRowEqual(image[i][j+1],image[i][j+2]) /*|| isRowEqual(image[i][j+1],image[i-1][j+1])*/){						push(bitString, 0x01);
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
						while(j<WIDTH-1 && !isRowEqual(image[i][j],image[i][j+1])){
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

void getBasis(const int &nBasis, const int &fromBasis,const int &toBasis, int ***basis){
	int **H;

	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	printf("nBasis = %d\n",nBasis);
	hadamard(H, nBasis);
	int cols = 8;
	int rows = nBasis/cols;

	//matrix serve per riarrangiare H
	int **matrix;
	matrix =(int**)malloc(rows*sizeof(int*));
	for(int i = 0; i<rows; i++)
		matrix[i]=(int*)malloc(cols*sizeof(int));
	int *pieciesOfCake;
	pieciesOfCake=(int*)malloc(nBasis*sizeof(int));
	
	for(int i = 0; i<nBasis; i++){
		//Conta numero di pezzi di torta
		for(int j = 0;j<nBasis; j++){
			matrix[j/cols][j%cols] = (H[i][j]+1)/2;
		}
		//Conta numero di pezzi di torta
		pieciesOfCake[i] = countIslands(matrix,rows,cols);
	}
	for(int i = 0; i<rows; i++)
		free(matrix[i]);
	free(matrix);
	for(int i = 0; i<nBasis; i++){
		for(int j = i +1; j<nBasis; j++){
			if(pieciesOfCake[i]>pieciesOfCake[j]){
				int tmp = pieciesOfCake[i];
				pieciesOfCake[i] = pieciesOfCake[j];
				pieciesOfCake[j] = tmp;
				for(int k = 0; k<nBasis; k++){
					tmp = H[i][k];
					H[i][k] =  H[j][k];
					H[j][k] =tmp;
				}
			}
		}
	}

	free(pieciesOfCake);



	//come inserire? nel senso binning? padding?/ transformazione?

	//int mult = 32/nBasis;//devo cercare la più vicina potenza del 2
	
	int logN = log(nBasis)/log(2);
	int mult=WIDTH/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2;
	for(int i = fromBasis; i <toBasis; i++){
		for(int j = 0; j<WIDTH;j++){
			
			if(j>(idxZeros-1) && j<(WIDTH-idxZeros)){
				int el =(H[i][(j-idxZeros)/mult]+1)/2;
				 for(int k = 0; k<HEIGHT; k++)
					basis[i-fromBasis][k][j] = el;
			}
			else for(int k = 0; k<HEIGHT; k++) basis[i-fromBasis][k][j] = 0;	
			
		}
		
	}
	
	for(int i = 0; i<nBasis; i++)
		free(H[i]);
	free(H);

}
int min(const int &a,const int &b){
	if(a>b)	return b;
	else return a;

}
int celing(const int &a, const int &b){
	if(a%b == 0)
		return a/b;
	return a/b +1;
}



int numberOfCakes(int ** matrix,const int &rows,const int &cols){
	int **tmp;
	tmp =(int **)malloc(rows*sizeof(int*));
	for(int i = 0; i<rows; i++){
		tmp[i]=(int *)malloc(cols*sizeof(int));
	}
	for(int i = 0; i<rows; i++){
		for(int j = 0; j<cols; j++) tmp[i][j] = 0;

	}

	//check around
	int cont = 0;
	for(int i = 0; i<rows; i++){
		for(int j = 0; j<cols; j++) {
			if(matrix[i][j]){//cerca a _|
				if((i== 0 && j == 0) ||
				( i>0 && j == 0 &&!matrix[i-1][j]) ||
				(i == 0 && j>0 && !matrix[i][j-1]) ||
				(i> 0 && j > 0 && !matrix[i][j-1] && !matrix[i-1][j])){
					cont++;
				}
				tmp[i][j]= cont;
			/* la configuarzione   _|_
						|
				non viene conssiderata qui la tengo in considerazione */
				for(int i = 0; i<rows; i++){
					for(int j = 0; j<cols; j++)	printf("%d ", tmp[i][j]);
					printf("\n");
					

				}
				if(i> 0 && j > 0 && tmp[i-1][j]!= 0 && tmp[i][j-1]!= 0 && tmp[i-1][j] !=tmp[i][j-1]){
					if(tmp[i-1][j]>tmp[i][j-1]) {
						tmp[i-1][j] = tmp[i][j-1];
						cont = tmp[i][j-1];
						}
					else{
						tmp[i][j-1] = tmp[i-1][j];
						cont = tmp[i][j-1];
						}
				}
			}
		}
	}
	printf("\n");
	for(int i = 0; i<rows; i++){
		for(int j = 0; j<cols; j++)	printf("%d ", tmp[i][j]);
		printf("\n");
		

	}
	for(int i = 0;i<rows;i++)
		free(tmp[i]);
	free(tmp);
	return cont;
}

//fonte:https://www.geeksforgeeks.org/find-number-of-islands/


int isSafe(int **M, int row, int col, bool **visited, int ROW, int COL) 
{ 
    // row number is in range, column number is in range and value is 1 
    // and not yet visited 
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL) && (M[row][col] && !visited[row][col]); 
} 
  
// A utility function to do DFS for a 2D boolean matrix. It only considers 
// the 8 neighbours as adjacent vertices 
void DFS(int **M, int row, int col, bool **visited, int ROW, int COL)
{ 
    // These arrays are used to get row and column numbers of 8 neighbours 
    // of a given cell 
    static int rowNbr[] = { -1, 0, 0, 1 }; 
    static int colNbr[] = { 0, -1, 1, 0 }; 
  
    // Mark this cell as visited 
    visited[row][col] = true; 
  
    // Recur for all connected neighbours 
    for (int k = 0; k < 4; ++k) 
        if (isSafe(M, row + rowNbr[k], col + colNbr[k], visited, ROW, COL)) 
            DFS(M, row + rowNbr[k], col + colNbr[k], visited, ROW, COL); 
} 
  
// The main function that returns count of islands in a given boolean 
// 2D matrix 
int countIslands(int **M,int ROW, int COL)
{ 
    // Make a bool array to mark visited cells. 
    // Initially all cells are unvisited 
    bool **visited;
    visited= (bool**)malloc(ROW * sizeof(bool*));
    for(int i = 0; i<ROW; i++) visited[i]= (bool*)malloc(COL * sizeof(bool));
    for(int i = 0; i<ROW; i++)
	for(int j = 0;j<COL; j++) visited[i][j]= 0;

  
    // Initialize count as 0 and travese through the all cells of 
    // given matrix 
    int count = 0; 
    for (int i = 0; i < ROW; ++i) 
        for (int j = 0; j < COL; ++j) 
            if (M[i][j] && !visited[i][j]) // If a cell with value 1 is not 
            { // visited yet, then new island found 
                DFS(M, i, j, visited, ROW, COL); // Visit all cells in this island. 
                ++count; // and increment island count 
            } 
    for(int i = 0; i<ROW; i++) free(visited[i]);
    free(visited);
    return count; 
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
	    (*image)[idx/width][idx%width]=data[i];
    }

}

