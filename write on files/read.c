#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZE_BUFFER 2049
int nDigit(int n){
	int cont;
	while (n != 0) {
		n /= 10;     // n = n/10
		cont++;
	    }
	return cont;
}
int ** getBasisHadamardFromTxt(int nBasis, const int *idx, const int szIdx){
	int **H;
	FILE *file;
	const char* mode = "r";
	char *extention =".txt";
	char * nBasisStr;
	snprintf (nBasisStr, nDigit(nBasis)+1, "%d",nBasis);
	char *fileName = malloc(strlen(nBasisStr) + strlen(extention) + 1);
	strcpy(fileName, nBasisStr);
    	strcat(fileName, extention);
	file = fopen(fileName,mode);//check open
	if(!file){
		printf("unable to open file!");
		return NULL;
	}
	H =(int **)malloc(szIdx*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	char r[SIZE_BUFFER];
	int i = 0;
	for(int k = 0; fgets(r,SIZE_BUFFER,file)!=NULL; k++) {//prendo solo quelle in idx!
		int id_el=idx[i];
		if(id_el== k) for(int j=0; j < nBasis; j++) {
			H[id_el][j] = r[2*i]-'0';
			i++;
			}
	}	
	fclose(file);
	return H;
}
int main(){
int idx[2] ={1,2};
int szIdx=2;
int nBasis=512;
//getBasisHadamardFromTxt(512,idx,2);
int **H;
	FILE *file;
	const char* mode = "r";
	char *extention =".txt";
	char * nBasisStr;
	snprintf (nBasisStr, nDigit(nBasis)+1, "%d",nBasis);
	char *fileName = malloc(strlen(nBasisStr) + strlen(extention) + 1);
	strcpy(fileName, nBasisStr);
    	strcat(fileName, extention);
	file = fopen(fileName,mode);//check open
	if(!file){
		printf("unable to open file!");
		return NULL;
	}
	H =(int **)malloc(szIdx*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	char r[SIZE_BUFFER];
	int i = 0;
	for(int k = 0; fgets(r,SIZE_BUFFER,file)!=NULL; k++) {//prendo solo quelle in idx!
		int id_el=idx[i];
		if(id_el== k) for(int j=0; j < nBasis; j++) {
			H[id_el][j] = r[2*i]-'0';
			i++;
			}
	}	
	fclose(file);
return 0;


}
