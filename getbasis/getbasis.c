#include "getbasis.h"
#include "ordering.h"
#define SIZE_BUFFER 2049

void getBasis(const int hadamard_raster, const int dim, const int *idx, const int szIdx, const int addBlank, int ***output){
	if(hadamard_raster == 1)
		getBasisHadamard(dim, idx, szIdx, addBlank, output);//output non so se magari devo passare in maniera diversa
	else if(hadamard_raster == 0)
		getBasisRaster(dim, idx, szIdx, addBlank, output);//output non so se magari devo passare in maniera diversa

}

int **ordering(const int nBasis, const int *idx, const int szIdx){
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
	int **output;
	output=(int **)malloc(szIdx*sizeof(int*));
	
	for(int i = 0; i <szIdx; i++){
		output[i]=(int*)malloc(nBasis*sizeof(int));
		int indexBasis = idx[i];
		printf("indexBasis = %d \n", indexBasis);
		for(int j = 0; j<nBasis; j++) output[i][j] = H[indexBasis][j];	
	}



	for(int i = 0; i<nBasis; i++)
		free(H[i]);
	free(H);
	return output;

}
void getBasisHadamard(const int nBasis, const int *idx, const int szIdx, const int addBlank, int ***basis){
	//come inserire? nel senso binning? padding?/ transformazione?

	//int mult = 32/nBasis;//devo cercare la più vicina potenza del 2
	int ** H;
	//H =  ordering(nBasis,idx,szIdx); // genera automaticamente
	H =  getBasisHadamardFromTxt(nBasis,idx,szIdx);//legge dal file
	int logN = log(nBasis)/log(2);
	int mult=WIDTH/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2;
	for(int cont = 0; cont <szIdx; cont++){
		int i;
		if(addBlank) i=cont*2;
		else i = cont;
		//printf(" i = %d\n", i);
		for(int j = 0; j<WIDTH;j++){
			if(j>(idxZeros-1) && j<(WIDTH-idxZeros)){
				int el =(H[cont][(j-idxZeros)/mult]+1)/2;
				 for(int k = 0; k<HEIGHT; k++)
					basis[i][k][j] = el; // prima era basis[i-fromBasis][k][j] = el; non mi ricordo perchè
			}
			else for(int k = 0; k<HEIGHT; k++) basis[i][k][j] = 0;	
			if(addBlank)	for(int k = 0; k<HEIGHT; k++) basis[i+1][k][j] = 0;
		}	
	}
	for(int i = 0; i<szIdx; i++)
		free(H[i]);
	free(H);


}


void getBasisRaster(const int dim, const int *idx, const int szIdx, const int addBlank, int ***basis){//possibile farlo a scatti o continuo
	for(int cont= 0; cont<szIdx; cont++){
		int k;
		if(addBlank) k = cont*2;
		else k = cont;
		int indexRaster = idx[k];
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				if(abs(indexRaster -j)<dim ) basis[k][i][j]=1;
				else basis[k][i][j]=0;
				if(addBlank) basis[k+1][i][j]=0;
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
int ** getBasisHadamardFromTxt(int nBasis, const int *idx, const int szIdx){
	int **H;
	H =(int **)malloc(szIdx*sizeof(int*));
	for(int i = 0; i<szIdx; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));
	FILE *file;
	const char* mode = "r";
	char *extention =".txt";
	char  *nBasisStr = intToString(nBasis);
	printf("n Basis = %d ",nBasis);
	//snprintf (nBasisStr, nDigit(nBasis)+1, "%d",nBasis);

	char *fileName = (char *)malloc(strlen(nBasisStr) + strlen(extention) + 1);
	strcpy(fileName, nBasisStr);
    	strcat(fileName, extention);
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

