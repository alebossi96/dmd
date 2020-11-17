#define SIZE 65

#include"dmd.h"
int command(hid_device *handle, const char &mode, const char &sequencebyte, const char &com1, const char &com2, const char *data, const size_t &sizeData){


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
	for(int i = 0; i<sizeData; i++){
		buffer[tot%SIZE]=data[i]-'0';//è un char non so cosa fare
		if(tot%SIZE == 0){
			printf("[");
			for(int j =0; j<SIZE; j++)
				printf("%d, ", buffer[j]);
			printf("]\n");
			if(!DEBUG){
				int res = hid_write(handle, buffer,SIZE);
				printf("written bytes = %d \n", res);
				checkForErrors(handle);
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
	if(!DEBUG){
		int res = hid_write(handle, buffer,SIZE);
		printf("written bytes = %d \n", res);
		checkForErrors(handle);
		}
	printf("[");
			
	for(int i =0; i<SIZE; i++)
		printf("%d, ", buffer[i]);
	printf("]\n");
	/*
	if((7+size_data)<=SIZE){
		int i;
		for(i =0; i<size_data;i++)
			buffer[7+i]=data[i]-'0';//è un char non so cosa fare
		for(i = i+7; i<SIZE; i++)
			buffer[i]=0x00;
		}
	*/
	

	return 0;	
}

void checkForErrors(hid_device *handle){
	if( hid_error(handle)==NULL)
		printf("errori");
		//cosa intende per string nella documentazione

}



int pow2_i(const int &exp){
	int res=1;
	for(int i =0; i<exp; i++)
		res*=2;
	return res;
}
char * convlen(int a, size_t l){//a<2^l
	/*
    This function converts a number "a" into a bit string of
    length "l".
	*/
	char * res;
	res = (char*)malloc(l * sizeof(char));
	l--;//mi muovo all'ultima posizione
	while(a!=1){
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
	printf("size =%d\n",size);
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
	checkForErrors(handle);
}
void stopSequence(hid_device *handle){
	char mode[1]={0};
	command(handle, 'w', 0x00, 0x1a, 0x1b, mode,1);
	checkForErrors(handle);
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

	command(handle, 'w',0x00,0x1a,0x31,(char*) bitsToBytes(string,48),6);
	checkForErrors(handle);
	free(im);
}
int pow_i(const int &b,const int &exp){
	int res=0;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}
void mergeImages( int ***images, int res[1080][1920][3]){
	

	for(int i=0; i<24; i++){
		for(int j=0; j<1080; j++){
			for(int k=0; k<1920;k++){
				res[j][k][(2-i/8)]+=images[i][j][k]*pow_i(2,i%8);	
			}		
		}
	}

}
void definePatterns(hid_device *handle, const int &index,const int &exposure,const int &bitdepth, const char *color,const int &triggerIn,const int &darkTime,const int &triggerOut,const int &patInd,const int &bitPos){
	char payload[13];
	int *index_ = bitsToBytes(convlen(index,16),16);
	payload[0]=index_[0];
	payload[1]=index_[1];
	free(index_);
	int *exposure_ = bitsToBytes(convlen(exposure,24),24);
	for(int i=0;i<3; i++)	payload[2+i] = exposure_[i];
	char optionsByte[8];
	optionsByte[3] = '1';
	char *bitDepth = convlen(bitdepth-1,3);
	for(int i = 0; i<3; i++) optionsByte[4+i] = bitDepth[i];
	for(int i = 0; i<3; i++) optionsByte[1+i] = color[i];
	if(triggerIn) optionsByte[0]='1';
	else optionsByte[0]='0';
	int *tmp;
	tmp =bitsToBytes(optionsByte,8);
	payload[6]= tmp[0];
	free(tmp);
	tmp = bitsToBytes(convlen(darkTime,24),24);
	for(int i = 0; i<3; i++) payload[7+i] = tmp[i];
	free(tmp);
	tmp = bitsToBytes(convlen(!triggerOut,8),8);
	payload[10] = tmp[0];
	free(tmp);
	char lastBits[16];
	char *patInd_;
	patInd_ = convlen(patInd,11);
	char *bitPos_;
	bitPos_ = convlen(bitPos,5);
	for(int i = 0; i<5; i++) lastBits[i] =bitPos_[i];
	for(int i = 0; i<11; i++) lastBits[i+5] =patInd_[i];
	tmp = bitsToBytes(lastBits,16);
	payload[11]= tmp[0];
	payload[12]=tmp[1];
	command(handle, 'w',0x00,0x1a,0x34,payload,13);
	checkForErrors(handle);
}
void defSequenceTest(hid_device *handle,int matrixes[1][1080][1920],int *exposure,int *trigger_in, int *dark_time, int *trigger_out, int repetition, const int &size){
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
			int mergedImagesint[10];	
			/*mergeImages(imageData,mergedImagesint);
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
			
			*/
		}
	}

}	
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
		int sizePkg= size/24+1;*/
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
void startSequence(hid_device *handle){
	char mode[1]={2};
	command(handle,'w',0x00,0x1a,0x24,mode,1);
        checkForErrors(handle);
}

void push(struct Node *head, int data){
	struct Node* newRef= (struct Node*)malloc(sizeof(struct Node));
	newRef->data =data;
	if(head == NULL){
		head = newRef;
		head->next=NULL;
	}
	newRef->next =head;
	head = newRef;
}
void push(struct List *head, int *data){
	struct List* newRef= (struct List*)malloc(sizeof(struct List));
	newRef->data =data;
	if(head == NULL){
		head = newRef;
		head->next=NULL;
	}
	newRef->next =head;
	head = newRef;
}
int isRowEqual(const int *a, const int *b){
	for(int i=0; i<3;i++)
		if(a[i]!=b[i]) return 0;
	return 1;
}

void setBmp(hid_device *handle,const int  &index,const int &size){
	char payload[6];
	char index_[16];
	char *tmp;
	tmp = convlen(index,5);
	for(int i =0; i<11; i++) index_[i] = '0';
	for(int i = 0; i<5; i++) index_[i+11] = tmp[i];
	free(tmp);
	int *tmp2;
	tmp2= bitsToBytes(tmp,16);
	for(int i = 0; i<2; i++) payload[i]= tmp2[i];
	free(tmp2);
	int *total;
	total = bitsToBytes(convlen(size,32),32);
	for(int i = 0; i<4; i++) payload[i+2]= tmp2[i];
	command(handle, 'w',0x00,0x1a,0x2a,payload,6);
}
void bmpLoad(hid_device *handle, const int *image, const int &size){
	int packNum= size/504 +1;
	int cont = 0;
	for(int i = 0; i<packNum; i++){
		if(i%100 == 0) printf("%d di %d\n", i, packNum);
		char *payload;
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
		payload= (char*)malloc((bits+2)*sizeof(char));
		for(int j = 0; j<2; j++) payload[i] = leng[i];
		for(int j = 0; j<bits; j++) payload[i+2] = image[i];
		command(handle, 'w',0x11,0x1a,0x2b, payload, bits+2);
		checkForErrors(handle);
	}

}

void newEncode(const int image[1080][1920][3], struct Node *bitString, int &byteCount){
	byteCount=48;
	push(bitString, 0x53);
	push(bitString, 0x70);
	push(bitString, 0x6c);
	push(bitString, 0x64);
	int *res= bitsToBytes(convlen(1920,16),16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	res= bitsToBytes(convlen(1080,16),16);
	push(bitString, res[0]);//dovrebbero essere solo 2 byte
	push(bitString, res[1]);
	free(res);
	res=bitsToBytes(convlen(0,32),32);
	struct Node *link;
	link = bitString;
	for(int i = 0; i<4; i++) push(bitString, res[i]);
	free(res);
	for(int i = 0; i<8; i++) push(bitString, 0xff);
	for(int i = 0; i<5; i++) push(bitString, 0x00);
	push(bitString, 0x02);
	push(bitString, 0x01);
	for(int i = 0; i<21; i++) push(bitString, 0x00);
	int n=0;
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
							j++;
							n=0;
						}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) || isRowEqual(image[i][j+1],image[i-1][j+1])){
							push(bitString, 0x01);
							byteCount++;
							push(bitString, image[i][j][0]);
							push(bitString, image[i][j][1]);
							push(bitString, image[i][j][2]);
							byteCount+=3;
							j++;
							n=0;					
						}else{//se j<1919e le condizioni isRowEqual non si verificano
							push(bitString, 0x00);
							byteCount++;
							struct Node *toAppend=NULL;
							while(j<1919 && !isRowEqual(image[i][j],image[i][j+1])){
								n++;
								push(toAppend, image[i][j][0]);
								push(toAppend, image[i][j][1]);
								push(toAppend, image[i][j][2]);
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
			}else{//if(i>0)
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
						j++;
						n=0;
					}else if(j>1917 || isRowEqual(image[i][j+1],image[i][j+2]) || isRowEqual(image[i][j+1],image[i-1][j+1])){						push(bitString, 0x01);
						byteCount++;
						push(bitString, image[i][j][0]);
						push(bitString, image[i][j][1]);
						push(bitString, image[i][j][2]);
						byteCount+=3;
						j++;
						n=0;					
					}else{//se j<1919e le condizioni isRowEqual non si verificano
						push(bitString, 0x00);
						byteCount++;
						struct Node *toAppend=NULL;
						while(j<1919 && !isRowEqual(image[i][j],image[i][j+1])){
							n++;
							push(toAppend, image[i][j][0]);
							push(toAppend, image[i][j][1]);
							push(toAppend, image[i][j][2]);
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
	while(byteCount!=0){
		push(bitString, 0x00);
		byteCount++;	
	}
	int size = byteCount;
	int *total;
	total = bitsToBytes(convlen(size,32),32);
	for(int i = 0; i<4; i++){
		link->data=total[i];
		link = link->next;
	}
	free(total);

}//function



