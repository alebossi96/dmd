/*
come faccio a scalare l'img?
1. dimensione fissate  prima -> rast
2. adatta -> Had

Controlla!

Fare 2D
*/


void genImgHorFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis)//riscala img
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
void genImgHorAdapt(const int **mat, int nBasis, int nMeas, int ***basis)//adapt img
{
    int dim = HEIGHT/nBasis;
    int offset = (HEIGHT-dim*nBasis)/2
	for(int i= 0; i<nMeas; i++)
    {
		for(int j = 0; j<WIDTH;j++)
        {
		    int h = 0;
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



void genImgOblFixed(const int **mat, int nBasis, int nMeas, int dim, int ***basis){//riscala img
	//possibile farlo a scatti o continuo
	for(int i= 0; i<nMeas; cont++){

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
			if(j<(WIDTH+HEIGHT){
				int el = mat[i][j/dim];
				 for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
					basis[i][i_x+k][i_y-k] = el;
					}
				}
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
}
void genImgOblAdapt(const int **mat, int nBasis, int nMeas, int ***basis){//riscala img
	//possibile farlo a scatti o continuo
	int dim = (WIDTH+HEIGHT)/nBasis;
	int offset = (WIDTH+HEIGHT-dim*nBasis)/2
	for(int i= 0; i<nMeas; cont++){

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
			if(j<(WIDTH+HEIGHT){
                if(j< offset || j > WIDTH+HEIGHT -offset)  int el = 0;
                else    int el = mat[i][j/dim];
		        for(int k = 0; k<lim; k++){ // k,j se sono in obliquo deve cambiare !
                    basis[i][i_x+k][i_y-k] = el;
                }
            }
			else for(int k = 0; k<lim; k++) basis[i][i_x+k][i_y-k] = 0;	  // k,j se sono in obliquo deve cambiare !
		}	
	}
}

