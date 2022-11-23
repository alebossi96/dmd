// SOURCES:
// hadamard: https://www.geeksforgeeks.org/generate-hadamard-matrix-of-given-order/
// DFS: https://www.geeksforgeeks.org/depth-first-search-or-dfs-for-a-graph/
// countIslands: https://www.geeksforgeeks.org/find-number-of-islands/

#include "ordering.h"

void hadamard(short int **matrix, const int nBasis){

	int logN = log(nBasis)/log(2);

	//if(nBasis%pow_i(2,logN) != 0) // il check di 2^n viene già fatto nel main quindi questo lo toglierei
    //    return;

	matrix[0][0] = 1;
	for(int i=0; i<logN; i++){
		int size = pow2_i(i);
		for(int j=0; j<size; j++){
			for(int k=0; k<size; k++){
				matrix [j+size][k+size] = -matrix[j][k];
				matrix [j][k+size] = matrix[j][k];
				matrix [j+size][k] = matrix[j][k];
			}
		}
	}
}

void DFS(short int **M, int row, int col, bool **visited, int ROW, int COL)
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

int countIslands(short int **M, int ROW, int COL)
{
    // Make a bool array to mark visited cells.
    // Initially all cells are unvisited
    bool **visited;
    visited= (bool**)malloc(ROW * sizeof(bool*));
    for(int i = 0; i<ROW; i++)
        visited[i] = (bool*)malloc(COL * sizeof(bool));
    for(int i = 0; i<ROW; i++)
        for(int j = 0;j<COL; j++)
            visited[i][j] = 0;

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

int isSafe(short int **M, int row, int col, bool **visited, int ROW, int COL){
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL) && (M[row][col] && !visited[row][col]);
}

int pow2_i(const int exp){
	int res=1;
	for(int i =0; i<exp; i++)
		res*=2;
	return res;
}

int pow_i(const int b, const int exp){
	int res=1;
	for(int i=0; i<exp; i++)
		res*=b;
	return res;
}

int min(const int a, const int b){
	if(a>b)	return b;
	else return a;

}

