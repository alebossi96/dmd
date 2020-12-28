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

