//fonte:https://www.geeksforgeeks.org/find-number-of-islands/
#include "ordering.h"

void cake_cutting(int **H, const int nBasis){
	/*
	int **H;

	H =(int **)malloc(nBasis*sizeof(int*));
	for(int i = 0; i<nBasis; i++)
		H [i] = (int *)malloc(nBasis*sizeof(int));

	printf("nBasis = %d\n",nBasis);
	hadamard(H, nBasis);
	*/
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
	/*
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
    */
}


int pow2_i(const int exp){
	int res=1;
	for(int i =0; i<exp; i++)
		res*=2;
	return res;
}
int pow_i(const int b,const int exp){
	int res=1;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}
void hadamard(int **matrix, const int nBasis){
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


