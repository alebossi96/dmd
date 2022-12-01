/*
come faccio a scalare l'img?
1. dimensione fissate  prima -> rast
2. adatta -> Had

Controlla!

Fare 2D
*/

#include"genimg.h"
void genImgHorLineFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis)//riscala img
{
	for(int i= 0; i<nMeas; i++)
    {
		for(int j = 0; j<WIDTH;j++)
        {
		    int h = 0;
		    for(int k = 0; k<nBasis && h<HEIGHT; k++)
		    {
		        for(int l = 0; l<dim && h<HEIGHT; l++, h++)
                    basis[i][h][j] = mat[i][k];
		    }
            if(h<HEIGHT) for(h = h; h<HEIGHT; h++) basis[i][h][j] = 0;
        }
    }	
}
void genImgHorLineAdapt(const int **mat, int nBasis, int nMeas, int ***basis)//adapt img
{
    int dim = HEIGHT/nBasis;
    int offset = (HEIGHT-dim*nBasis)/2;
	for(int i= 0; i<nMeas; i++)
    {
		for(int j = 0; j<WIDTH;j++)
        {
		    for(int h = 0; h<HEIGHT; h++)
		    {
		        if(h< offset || h> HEIGHT-offset)
		            basis[i][h][j] = 0;
		        else
		            basis[i][h][j] = mat[i][(h-offset)/dim];
		    }
        }
    }	
}



void genImgOblLineFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis){//riscala img
	//possibile farlo a scatti o continuo
	for(int i= 0; i<nMeas; i++){

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
			if(j<(WIDTH+HEIGHT)){
				int el = mat[i][j/dim];
				 for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
					basis[i][i_x+k][i_y-k] = el;
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
}
void genImgOblLineAdapt(const int **mat, int nBasis, int nMeas, int ***basis){//riscala img
	//possibile farlo a scatti o continuo
	int dim = (WIDTH+HEIGHT)/nBasis;
	int offset = (WIDTH+HEIGHT-dim*nBasis)/2;
	for(int i= 0; i<nMeas; i++){

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
			if(j<(WIDTH+HEIGHT)){
			    int el;
                if(j< offset || j > WIDTH+HEIGHT -offset)
                    el = 0;
                else    
                    el = mat[i][j/dim];
		        for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
                    basis[i][i_x+k][i_y-k] = el;
                }
            }
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
}
void genImgHor2D(const int **mat, int nBasis, int nMeas, int xStart, int yStart, int dimX, int dimY, int xStop, int yStop, int ***basis){
    for(int idx = 0; idx<nMeas; idx ++){
        int i_b = 0;
        int j_b = 0;
        for(int i = 0; i<WIDTH; i++){
            for(int j = 0; j<HEIGHT; j++){
                if(i<xStart || j<yStart || i>xStop|| j> yStop){
                    basis[idx][i][j] = 0;
                    continue;
                }
                basis[idx][i][j] = mat[idx][i_b];
                if(dimX*(i_b+1)>(j-xStart) )
                    i_b++;
                if(j== xStop-1 && j_b<dimY)
                    i_b-= nWidth;
                if(j_b == dimY)
                    j_b = 0;
            }
        }
    }
}
void genImgHor2DFixed(const int **mat, int nBasis, int nMeas, int xStart, int yStart, int dimX, int dimY, int ***basis){
    int nHeight = (int) sqrt(nBasis);
    int nWidth;
    if(nBasis == nHeight*nHeight)
        nWidth = nHeight; //quadrato
    else
        nWidth = 2*nHeight;//rettangolo
    int xStop = nWidth*dimX + xStart;
    int yStop = nHeight*dimY + yStart;
    genImgHor2D(mat, nBasis, nMeas, xStart, yStart, dimX, dimY, xStop, yStop, basis)
}
void genImgHor2DAdapt(const int **mat, int nBasis, int nMeas, int xStart, int yStart, int xStop, int yStop, int ***basis){
    //rStop in zona
    int nHeight = (int) sqrt(nBasis);
    int nWidth;
    if(nBasis == nHeight*nHeight)
        nWidth = nHeight; //quadrato
    else
        nWidth = 2*nHeight;//rettangolo
    int dimX = round((xStop-xStart)/(1.*nWidth));
    int dimY = round((yStop-yStart)/(1.*nHeight));
    genImgHor2D(mat, nBasis, nMeas, xStart, yStart, dimX, dimY, xStop, yStop, basis)
}
int min(const int a, const int b){
	if(a>b) return b;
	return a;
}

