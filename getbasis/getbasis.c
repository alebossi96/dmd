
#include "getbasis.h"

void getBasis(const int hadamard_raster, const int dim, const int *idx, const int szIdx, int compressImage, unsigned char ***output){
    printf("Generating pattern\n");
	if(hadamard_raster == 1)
		getBasisHadamard(dim, idx, szIdx, compressImage, output);
	else if(hadamard_raster == 0)
		getBasisRaster(dim, idx, szIdx, compressImage, output);
	else if(hadamard_raster == 2)
		getBasisOnes(szIdx, output);
	else if(hadamard_raster == 3)
		getBasisZeros(szIdx, output);
	else if(hadamard_raster == 4)
		getBasisNotchFilter(idx, dim, output);
	else if(hadamard_raster == 5)
		getBasisHadamardHorizontal(dim, idx, szIdx, compressImage, output);
	else if(hadamard_raster == 6)
		getBasisRasterHorizontal(dim, idx, szIdx, compressImage, output);
	else if(hadamard_raster == 7)
		getBasisAddOneLineHorizontal(dim, idx, szIdx, compressImage, output);
	else if(hadamard_raster == 8)
		getBasisBandPass(idx, dim, output);
	else if(hadamard_raster == 9)
		getBasisAddOneLineObli(dim, idx, szIdx, compressImage, output);
    else if(hadamard_raster == 10)
		getBasisHadamard2D(dim, idx, szIdx, output);
	//TODO aumentare le basi se meno di un tot (CIOE'?)
	//to get band pass spatial filter use raster with only 1 base! and select dimension
}

void getBasisHadamard(const int nBasis, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){

	//come inserire? nel senso binning? padding?/ transformazione? (???)

	short int ** H;
	int mode = 0;   // 0 = Hadamard matrix ordering,        TODO (per CS): inserire la scelta dell'uso di CC nel wizard e a quale base fermarsi
                    // 1 = cake-cutting ordering,
                    // 2 = read bases from txt file

    // generation of the Hadamard matrix (and ordering)
	H =(short int **)malloc(nBasis*sizeof(short int*));
    for(int i=0; i<nBasis; i++)
        H[i] = (short int *)malloc(nBasis*sizeof(short int));
    if(mode == 2)
        getBasisHadamardFromTxt(H, nBasis, idx, szIdx); // read from .txt file
    else{
        hadamard(H, nBasis);
        if (mode == 1)
            ordering(H, nBasis, idx, szIdx); // cake-cutting
    }

    // print the created matrix
    printf("\n");
	for(int i=0; i<szIdx; i++ ){
		for(int j = 0; j<nBasis; j++){
            printf("%d ", H[i][j]);
		}
		printf("\n");
	}
    printf("\n");

	int dimCompression = 200; // N of central pixels to open (if compression = 1)
	int logN = log(nBasis)/log(2);
	int mult = (WIDTH+HEIGHT)/pow2_i(logN);  // pow2_i(logN) non è nBasis ?
	int idxZeros = (WIDTH+HEIGHT-mult*nBasis)/2; // trova dove partire (le basi di Hadamard sono di dimensioni 2^n quindi i pixel utilizzati devono essere 2^n)
	for(int cont = 0; cont<szIdx; cont++){ // count on the basis
		int i = cont;

		for(int j = 0; j<WIDTH+HEIGHT; j++){
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
				int el =(H[cont][(j-idxZeros)/mult]+1)/2;           // we use a modification of Hadamard matrix: H* = (H+ones)/2
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

void ordering(short int** H, const int nBasis, const int *idx, const int szIdx){

	int cols = 8;               // non vanno variate in base a nBasis per ottenere sempre una matrice quadrata?
	int rows = nBasis/cols;

	short int **matrix; // matrix serve per contare i pieces of cake e riarrangiare H
	matrix = (short int**)malloc(rows*sizeof(short int*));
	for(int i = 0; i<rows; i++)
		matrix[i] = (short int*)malloc(cols*sizeof(short int));
	int *pieciesOfCake;
	pieciesOfCake = (int*)malloc(nBasis*sizeof(int));
	for(int i=0; i<nBasis; i++){
		for(int j=0; j<nBasis; j++){
			matrix[j/cols][j%cols] = (H[i][j]+1)/2;
		}
		pieciesOfCake[i] = countIslands(matrix, rows, cols); // Conta il numero di pezzi di torta per la base nella riga i
	}
	for(int i=0; i<rows; i++)
		free(matrix[i]);
	free(matrix);

	for(int i=0; i<nBasis; i++){
		for(int j=i+1; j<nBasis; j++){
			if(pieciesOfCake[i]>pieciesOfCake[j]){
				int tmp = pieciesOfCake[i];
				pieciesOfCake[i] = pieciesOfCake[j];
				pieciesOfCake[j] = tmp;
				for(int k=0; k<nBasis; k++){ // scambia le righe mettendole in ordine crescente di pieecies of cake
					tmp = H[i][k];
					H[i][k] = H[j][k];
					H[j][k] = tmp;
				}
			}
		}
	}
	free(pieciesOfCake);

	// A QUESTO PUNTO H è GIA ORDINATA
	/*
	int **output;
	output = (int **)malloc(szIdx*sizeof(int*));

	for(int i=0; i<szIdx; i++){
		output[i] = (int*)malloc(nBasis*sizeof(int));
		int indexBasis = idx[i];
		// printf("indexBasis = %d \n", indexBasis);
		for(int j=0; j<nBasis; j++)
            output[i][j] = H[indexBasis][j];
	}
	for(int i=0; i<nBasis; i++)
		free(H[i]);
	free(H);
	return output;
	*/

	return;
}

void getBasisHadamardFromTxt(short int **H, const int nBasis, const int *idx, const int szIdx){

	FILE *file;
	char *mode = "r";
	char *extention = ".txt";
	char *nBasisStr = intToString(nBasis);
	char *folderName = "basis/";

	//printf("n Basis = %d ",nBasis);
	//snprintf (nBasisStr, nDigit(nBasis)+1, "%d",nBasis);

	char *fileName = concatenate(folderName, nBasisStr, extention);
	free(nBasisStr);
	file = fopen(fileName, mode);
	free(fileName);
	if(!file){ //check open
		printf("*** Error: unable to open basis txt file!");
		printf("Press any key to exit.\n");
        getchar();
		exit(1);
	}

	char r[SIZE_BUFFER];
	int i = 0;
	// printf("da txt\n");
	for(int k=0; fgets(r,SIZE_BUFFER,file)!=NULL && i<szIdx; k++) { // prendo solo quelle in idx!
		int id_el = idx[i];
		if(id_el == k){
			for(int j=0; j<nBasis; j++) {
				H[i][j] = r[2*j]-'0'; // conversion ASCII -> bit
				// printf("%d ", H[i][j]);
			}
			// printf("\n");
			i++;
		}
	}
	fclose(file);
	return;
}

void getBasisRaster(const int dim, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){

	//possibile farlo a scatti o continuo (???)

	int idxZeros = 0; //(WIDTH+HEIGHT)/2;
	int dimCompression = 400;

	for(int cont=0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j=0; j<WIDTH+HEIGHT; j++){
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
						else
                            basis[i][i_x+k][i_y-k] = 0;
					}
					else
						basis[i][i_x+k][i_y-k] = el;
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}
	}
}

void getBasisOnes(const int sz, unsigned char ***basis){
    for(int k=0; k<sz; k++){
		for(int i=0; i<HEIGHT; i++){
			for(int j=0; j<WIDTH; j++){
				basis[k][i][j] = 1;
			}
		}
	}
}

void getBasisZeros(const int sz, unsigned char ***basis){
    for(int k=0; k<sz; k++){
		for(int i=0; i<HEIGHT; i++){
			for(int j=0; j<WIDTH; j++){
				basis[k][i][j] = 0;
			}
		}
	}
}

void getBasisNotchFilter(const int *idx, const int dim, unsigned char ***basis){

		//possibile farlo a scatti o continuo (???)

	int idxZeros = 0; //(WIDTH+HEIGHT)/2;
	for(int cont=0; cont<24; cont++){//deve essere sempre 24!
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

void getBasisHadamardHorizontal(const int nBasis, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){

	//come inserire? nel senso binning? padding?/ transformazione? (???)

	short int ** H;
	int mode = 0;   // 0 = Hadamard matrix ordering,        TODO (per CS): inserire la scelta dell'uso di CC nel wizard e a quale base fermarsi
                    // 1 = cake-cutting ordering,
                    // 2 = read bases from txt file

    // generation of the Hadamard matrix (and ordering)
	H =(short int **)malloc(nBasis*sizeof(short int*));
    for(int i=0; i<nBasis; i++)
        H[i] = (short int *)malloc(nBasis*sizeof(short int));
    if(mode == 2)
        getBasisHadamardFromTxt(H, nBasis, idx, szIdx); // read from .txt file
    else{
        hadamard(H, nBasis);
        if (mode == 1)
            ordering(H, nBasis, idx, szIdx); // cake-cutting
    }

    // print Hadamard matrix
	for(int i = 0; i<szIdx; i++ ){
		for(int j = 0; j<nBasis; j++) printf("%d ", H[i][j]);
		printf("\n");
	}

	int logN = log(nBasis)/log(2);
	int mult=(WIDTH)/pow2_i(logN);
	int idxZeros = (WIDTH-mult*nBasis)/2; //trova dove partire ricordtati che le basi di Hadamard sono di dimensioni 2^n
	for(int cont = 0; cont <szIdx; cont++){ //count on the basis
		int i = cont;
		for(int j = 0; j<WIDTH; j++){
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

void getBasisRasterHorizontal(const int dim, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){

	//possibile farlo a scatti o continuo (???)

	int idxZeros = 0; //(WIDTH+HEIGHT)/2;
	for(int cont=0; cont<szIdx; cont++){
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

void getBasisAddOneLineHorizontal(const int dim, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){

	//possibile farlo a scatti o continuo (??)

	int idxZeros = 0; //(WIDTH+HEIGHT)/2;
	for(int cont= 0; cont<szIdx; cont++){
		int i = cont;
		int indexRaster = idx[i];
		for(int j = 0; j<WIDTH; j++){
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

void getBasisBandPass(const int *idx, const int dim, unsigned char ***basis){
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

void getBasisAddOneLineObli(const int dim, const int *idx, const int szIdx, int compressImage, unsigned char ***basis){


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

void getBasisHadamard2D(const int nBasis, const int *idx, const int szIdx, unsigned char ***basis ){

    short int ** H;
	int mode = 0;   // 0 = Hadamard matrix ordering,        TODO (per CS): inserire la scelta dell'uso di CC nel wizard e a quale base fermarsi
                    // 1 = cake-cutting ordering,
                    // 2 = read bases from txt file

    // generation of the Hadamard matrix (and ordering)
	H =(short int **)malloc(nBasis*sizeof(short int*));
    for(int i=0; i<nBasis; i++)
        H[i] = (short int *)malloc(nBasis*sizeof(short int));
    if(mode == 2)
        getBasisHadamardFromTxt(H, nBasis, idx, szIdx); // read from .txt file
    else{
        hadamard(H, nBasis);
        if (mode == 1)
            ordering(H, nBasis, idx, szIdx); // cake-cutting
    }

    // print Hadamard matrix
	for(int i = 0; i<szIdx; i++ ){
		for(int j = 0; j<nBasis; j++) printf("%d ", H[i][j]);
		printf("\n");
	}

	int sq = sqrt(nBasis);
    int multW = (WIDTH)/sq;
    int multH = (HEIGHT)/sq;
    printf("multW %d, multH %d\n", multW, multH);
    int idxZerosW = (WIDTH-multW*nBasis)/2;      // pixel saltati
    int idxZerosH = (HEIGHT-multH*nBasis)/2;
    int cont = 0;
    int pos = 0;
    int row = 0;
    int col = 0;
    for(cont=0; cont<szIdx; cont++){
        // select the basis
        for(pos=0; pos<szIdx; ){
            // select basis element
            //printf("pos %d\n", pos);
            for(col=0; col<sq; col++){
                for(row=0; row<sq; row++){
                    //printf("j limits %d : %d\n", multH*row, multH*(row+1));
                    //printf("k limits %d : %d\n", multW*col, multW*(col+1));
                    for(int j=multH*row; j<multH*(row+1); j++){
                        for(int k=multW*col; k<multW*(col+1); k++){
                            // fill the image (rows and then cols)
                            //printf("cont %d, pos %d, j %d, k %d\n", cont, pos, j ,k);
                            basis[cont][j][k] = (H[cont][pos]+1)/2;
                        }
                    }
                    pos++;
                }
            }
		}
	}
	for(int i = 0; i<szIdx; i++)
		free(H[i]);
	free(H);

}

int nDigit(int n){
	int cont = 0;
	while (n != 0){
		n = n/10;
		cont++;
    }
	return cont;
}

char *intToString(int n){
	int nD = nDigit(n)+1;
	char * res = (char *)malloc(nD*sizeof(char));
	for(int i=0; i<nD-1; i++)
		res[i]='0';
	res[nD-1]='\0';
	for(int i=0; i<nD-1; i++){
		res[nD-2-i] = n%10+'0';
		n/=10;
	}
	// printf("res = %s",res);
	return res;
}

char *concatenate(char *a, char *b, char *c){
  int size = strlen(a) + strlen(b) + strlen(c) + 1;
  char *str = malloc(size);
  strcpy(str, a);
  strcat(str, b);
  strcat(str, c);
  return str;
}
