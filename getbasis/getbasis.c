#include "getbasis.h"
#include "ordering.h"
#define SIZE_BUFFER 2049
/*
void getBasis(const int hadamard_raster, const int dim, const int *idx, const int szIdx,int compressImage, int ***output){
	if(hadamard_raster == 1)
		getBasisHadamard(dim, idx, szIdx,compressImage, output);//output non so se magari devo passare in maniera diversa
	else if(hadamard_raster == 0)
		getBasisRaster(dim, idx, szIdx,compressImage, output);//output non so se magari devo passare in maniera diversa
	
	else if(hadamard_raster == 2)
		getBasisOnes(szIdx, output);
	else if(hadamard_raster == 3)
		getBasisZeros(szIdx, output);
	else if(hadamard_raster == 4)
		getBasisNotchFilter(idx,dim,output);
	else if(hadamard_raster == 5)
		getBasisHadamardHorizontal(dim, idx, szIdx,compressImage, output);
	else if(hadamard_raster == 6)
		getBasisRasterHorizontal(dim, idx, szIdx,compressImage, output);
	else if(hadamard_raster == 7)
		getBasisAddOneLineHorizontal(dim, idx, szIdx,compressImage, output);
	else if(hadamard_raster == 8)
		getBasisBandPass(idx,dim,output);
	else if(hadamard_raster == 9)
		getBasisAddOneLineObli(dim, idx, szIdx,compressImage, output);		
	//TODO aumentare le basi se meno di un tot
	//to get band pass spatial filter use raster with only 1 base! and select dimension
}
*/
void getBasis(const int type, const int orientation,  const int dim, const int nMeas, const int offset, const int order, int ***output){
    int **mat;
    mat =(int**)malloc(nMeas*sizeof(int*));
    for (i = 0; i<nLine; i++){
        mat[i] =(int*)malloc(dim*sizeof(int));
    }
    
    if(type == 0)
        genMatRast(mat, dim, nMeas, offset);
    else if (type == 1)
        getMatHad(mat, dim, nMeas, order);
    else if(type == 2) 
        genMatOne(mat, dim,nMeas);
    else if(type == 3) 
		genMatZero(mat, dim, nMeas);
		
    for (i = 0; i<nLine; i++){
        free(mat[i]);
    }
    free(mat);
}


int min(const int a, const int b){
	if(a>b) return b;
	return a;
}
void getBasisHadamard(const int nBasis, const int *idx, const int szIdx,int compressImage, int ***basis){
	//come inserire? nel senso binning? padding?/ transformazione?
	int ** H;
	//H =  ordering(nBasis,idx,szIdx); // genera automaticamente
	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	hadamard(H, nBasis);
	//H =  getBasisHadamardFromTxt(nBasis,idx,szIdx);//legge dal file
	for(int i = 0; i<szIdx; i++ ){
		for(int j = 0; j<nBasis; j++) printf("%d ", H[i][j]);
		printf("\n");
	}

	int dimCompression = 200;
	int logN = log(nBasis)/log(2);
	int mult=(WIDTH+HEIGHT)/pow2_i(logN);
	int idxZeros = (WIDTH+HEIGHT-mult*nBasis)/2; //trova dove partire ricordtati che le basi di Hadamard sono di dimensioni 2^n	
	for(int cont = 0; cont <szIdx; cont++){ //count on the basis
		int i = cont;
		
		for(int j = 0; j<WIDTH+HEIGHT;j++){
			int i_y;
			int i_x;
			int lim = HEIGHT;
			
			if(j<HEIGHT || j >= WIDTH)
				lim = min(j+1, HEIGHT + WIDTH - j );
			if(j<WIDTH){
				i_y = j; 
				i_x = 0;
			 }
			if(j>=WIDTH){
				i_y = WIDTH-1; 
				i_x = j-WIDTH;
			 }
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el =(H[cont][(j-idxZeros)/mult]+1)/2;
				 for(int k = 0; k<lim; k++){
					if(compressImage){
						if(abs(k-j/2)<dimCompression/2)
							basis[i][i_x+k][i_y-k] = el;
						else basis[i][i_x+k][i_y-k] = 0;
					}
					else
						basis[i][i_x+k][i_y-k] = el;
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
	for(int i = 0; i<szIdx; i++)
		free(H[i]);
	free(H);


}


void getBasisRaster(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis){
	//possibile farlo a scatti o continuo
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	int dimCompression = 400;
	for(int cont= 0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j = 0; j<WIDTH+HEIGHT;j++){
			int i_y;
			int i_x;
			int lim = HEIGHT;
			
			if(j<HEIGHT || j >= WIDTH)
				lim = min(j+1/*io sicuro*/, HEIGHT + WIDTH - j );
			if(j<WIDTH){
				i_y = j; 
				i_x = 0;
			 }
			if(j>=WIDTH){
				i_y = WIDTH-1; 
				i_x = j-WIDTH;
			 }
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j>=dim*indexRaster && j<dim*(indexRaster + 1))
					el = 1;
				else el = 0;
				 for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
									
					if(compressImage){
						if(abs(k-j/2)<dimCompression/2)
							basis[i][i_x+k][i_y-k] = el;
						else basis[i][i_x+k][i_y-k] = 0;
					}
					else
						basis[i][i_x+k][i_y-k] = el;
					
					
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
}


void getBasisOnes(const int sz, int ***basis){
		for(int k= 0; k<sz; k++){
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				basis[k][i][j]=1;
			}	
		}
	}
}

void getBasisZeros(const int sz, int ***basis){
		for(int k= 0; k<sz; k++){
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				basis[k][i][j]=0;
			}	
		}
	}
}

int nDigit(int n){
	int cont = 0;
	while (n != 0) {
		n =n / 10;     // n = n/10
		cont++;
	    }
	return cont;
}
char *intToString(int n){
	int nD = nDigit(n)+1;
	char * res = (char *)malloc(nD*sizeof(char));
	for(int i= 0; i<nD-1;i++)
		res[i]='0';
	res[nD-1]='\0';
	
	for(int i= 0; i<nD-1;i++){
		res[nD-2-i] = n%10+'0';
		n/=10;
	} 
	
	printf("res = %s",res);
	return res;
}
char *concatenate(char *a, char *b, char *c)
{
  int size = strlen(a) + strlen(b) + strlen(c) + 1;
  char *str = malloc(size);
  strcpy (str, a);
  strcat (str, b);
  strcat (str, c); 

  return str;
}



int ** getBasisHadamardFromTxt(int nBasis, const int *idx, const int szIdx){
	int **H;
	H =(int **)malloc(szIdx*sizeof(int*));
	for(int i = 0; i<szIdx; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));
	FILE *file;
	char* mode = "r";
	char *extention =".txt";
	char  *nBasisStr = intToString(nBasis);
	char *folderName ="basis/";
	printf("n Basis = %d ",nBasis);
	//snprintf (nBasisStr, nDigit(nBasis)+1, "%d",nBasis);

	char *fileName = concatenate(folderName, nBasisStr, extention);
	free(nBasisStr);
	file = fopen(fileName,mode);//check open
	free(fileName);
	if(!file){
		printf("unable to open file!");
		return NULL;
	}

	char r[SIZE_BUFFER];
	int i = 0;
	printf("da txt\n");	
	for(int k = 0; fgets(r,SIZE_BUFFER,file)!=NULL && i< szIdx; k++) {//prendo solo quelle in idx!
		int id_el=idx[i];
		if(id_el== k) {
			for(int j=0; j < nBasis; j++) {
				H[i][j] = r[2*j]-'0';
				printf("%d ", H[i][j]);			
			}
			printf("\n");	
			i++;
		}
	}
		
	fclose(file);
	return H;
}

void getBasisNotchFilter(const int *idx,const int dim, int ***basis){
		//possibile farlo a scatti o continuo
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	for(int cont= 0; cont<24; cont++){//deve essere sempre 24!
		int i = cont;
		int indexRaster = idx[0];
		for(int j = 0; j<WIDTH+HEIGHT;j++){
			int i_y;
			int i_x;
			int lim = HEIGHT;
			
			if(j<HEIGHT || j >= WIDTH)
				lim = min(j+1/*io sicuro*/, HEIGHT + WIDTH - j );
			if(j<WIDTH){
				i_y = j; 
				i_x = 0;
			 }
			if(j>=WIDTH){
				i_y = WIDTH-1; 
				i_x = j-WIDTH;
			 }
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j>=dim*indexRaster && j<dim*(indexRaster + 1))
					el = 0; //inverso di raster
				else el = 1;
				 for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
									
					basis[i][i_x+k][i_y-k] = el;
					
					
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 1;	  // k,j se sono in obliquo deve cambiare !
		}	
	}

}
void getBasisHadamardHorizontal(const int nBasis, const int *idx, const int szIdx,int compressImage, int ***basis){
	//come inserire? nel senso binning? padding?/ transformazione?
	int ** H;
	//H =  ordering(nBasis,idx,szIdx); // genera automaticamente
	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	hadamard(H, nBasis);
	//H =  getBasisHadamardFromTxt(nBasis,idx,szIdx);//legge dal file
	for(int i = 0; i<szIdx; i++ ){
		for(int j = 0; j<nBasis; j++) printf("%d ", H[i][j]);
		printf("\n");
	}

	int logN = log(nBasis)/log(2);
	int mult=(WIDTH)/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2; //trova dove partire ricordtati che le basi di Hadamard sono di dimensioni 2^n	
	for(int cont = 0; cont <szIdx; cont++){ //count on the basis
		int i = cont;
		for(int j = 0; j<WIDTH;j++){
			int el =(H[cont][(j-idxZeros)/mult]+1)/2;
			 for(int k = 0; k<HEIGHT; k++){
				basis[i][k][j] = el;
				}
		}	
	}
	for(int i = 0; i<szIdx; i++)
		free(H[i]);
	free(H);
}


void getBasisRasterHorizontal(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis){
	//possibile farlo a scatti o continuo
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	for(int cont= 0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j = 0; j<WIDTH;j++){
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j>=dim*indexRaster && j<dim*(indexRaster + 1)) el = 1;
				else el = 0;
				 for(int k = 0; k<HEIGHT; k++) // k,j se sono in obliquo deve cambiare !
					basis[i][k][j] = el;	
					
			}
		}	
	}



}
void getBasisAddOneLineHorizontal(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis){

	//possibile farlo a scatti o continuo
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	for(int cont= 0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j = 0; j<WIDTH;j++){
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j<dim*(indexRaster + 1)) el = 1;
				else el = 0;
				 for(int k = 0; k<HEIGHT; k++) // k,j se sono in obliquo deve cambiare !
					basis[i][k][j] = el;	
					
			}
		}	
	}



}
void getBasisAddOneLineObli(const int dim, const int *idx, const int szIdx,int compressImage, int ***basis){


	//possibile farlo a scatti o continuo
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	int dimCompression = 400;
	for(int cont= 0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j = 0; j<WIDTH+HEIGHT;j++){
			int i_y;
			int i_x;
			int lim = HEIGHT;
			
			if(j<HEIGHT || j >= WIDTH)
				lim = min(j+1/*io sicuro*/, HEIGHT + WIDTH - j );
			if(j<WIDTH){
				i_y = j; 
				i_x = 0;
			 }
			if(j>=WIDTH){
				i_y = WIDTH-1; 
				i_x = j-WIDTH;
			 }
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j<dim*(indexRaster + 1))
					el = 1;
				else el = 0;
				 for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
									
					if(compressImage){
						if(abs(k-j/2)<dimCompression/2)
							basis[i][i_x+k][i_y-k] = el;
						else basis[i][i_x+k][i_y-k] = 0;
					}
					else
						basis[i][i_x+k][i_y-k] = el;
					
					
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}





}
void getBasisBandPass(const int *idx,const int dim, int ***basis){
	int idxZeros = 0; //(WIDTH+HEIGHT)/2; 
	for(int cont= 0; cont<24; cont++){
		int i = cont;
		int indexRaster = idx[0];
		for(int j = 0; j<WIDTH;j++){
			if(j>(idxZeros-1) && j<(WIDTH+HEIGHT-idxZeros)){
				int el;
				if(j<dim*(indexRaster + 1)) el = 1;
				else el = 0;
				 for(int k = 0; k<HEIGHT; k++) // k,j se sono in obliquo deve cambiare !
					basis[i][k][j] = el;	
					
			}
		}	
	}

}
