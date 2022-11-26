#include<stdlib.h>
#include "ordering.h"

void genMatRast(int ** mat, int nLine,int nMeas,int offset){
    int i, j;

    for(i = 0; i<nMeas; i++){
      for(j = 0; j<nLine; j++){// come fare lui?
            if(i+offset == j)
                mat[i,j] = 1;
        }
    }
}

void getMatHad(int ** mat,int nBasis, int nMeas, int ordering){
    int ** H;
	//H =  ordering(nBasis,idx,szIdx); // genera automaticamente
	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

    if(ordering == 0)
        hadamard(H, nBasis);
    /*else
        H =  ordering(nBasis,idx,szIdx); //TODO combiare idx perchè ridondante & inserire in ordering*/
    for(i = 0; i<nMeas; i++){
      for(j = 0; j<nLine; j++){
            mat[i,j] = H[i,j]+1;
        }
    }

	for(int i = 0; i<nBasis; i++)
		free(H[i]);
    free(H);

}
void genMatOne(int ** mat, int nLine,int nMeas){
    int i, j;

    for(i = 0; i<nMeas; i++){
      for(j = 0; j<nLine; j++){
            mat[i,j] = 1;
        }
    }
}
void genMatZero(int ** mat, int nLine,int nMeas){
    int i, j;

    for(i = 0; i<nMeas; i++){
      for(j = 0; j<nLine; j++){
            mat[i,j] = 0;
        }
    }
}

