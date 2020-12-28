#include "getbasis.h"
#include "ordering.h"

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
