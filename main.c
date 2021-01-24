#include"measure.h"
int main(){
	struct DMD dmd;
	initDMD(&dmd);
	moveDMD(dmd);
	closeDMD(&dmd);
	return 0;
}


