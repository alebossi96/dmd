#include "getbasis.h"
#include "ordering.h"

//void getBasis(const int nBasis, const int fromBasis,const int toBasis, int ***basis){
void getBasis(const int hadamard_raster, const int dim, const int *idx, const int szIdx, int ***output){
	if(hadamard_raster == 1)
		getBasisHadamard(dim, idx, szIdx, output);//output non so se magari devo passare in maniera diversa
	else if(hadamard_raster == 0)
		getBasisRaster(dim, idx, szIdx, output);//output non so se magari devo passare in maniera diversa

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
		for(int j = 0; j<nBasis; j++) output[i][j] = H[indexBasis][j];	
	}



	for(int i = 0; i<nBasis; i++)
		free(H[i]);
	free(H);
	return output;

}
void getBasisHadamard(const int nBasis, const int *idx, const int szIdx, int ***basis){
	//come inserire? nel senso binning? padding?/ transformazione?

	//int mult = 32/nBasis;//devo cercare la più vicina potenza del 2
	int ** H;
	H =  ordering(nBasis,idx,szIdx);
	int logN = log(nBasis)/log(2);
	int mult=WIDTH/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2;
	for(int i = 0; i <szIdx; i++){
		for(int j = 0; j<WIDTH;j++){
			if(j>(idxZeros-1) && j<(WIDTH-idxZeros)){
				int el =(H[i][(j-idxZeros)/mult]+1)/2;
				 for(int k = 0; k<HEIGHT; k++)
					basis[i][k][j] = el; // prima era basis[i-fromBasis][k][j] = el; non mi ricordo perchè
			}
			else for(int k = 0; k<HEIGHT; k++) basis[i][k][j] = 0;		
		}	
	}
	for(int i = 0; i<szIdx; i++)
		free(H[i]);
	free(H);


}


void getBasisRaster(const int dim, const int *idx, const int szIdx, int ***basis){//possibile farlo a scatti o continuo
	for(int k= 0; k<szIdx; k++){
		int indexRaster = idx[k];
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				if(abs(indexRaster -j)<dim ) basis[k][i][j]=1;
				else basis[k][i][j]=0;
			}	
		}
	}
}

