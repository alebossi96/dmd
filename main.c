#include"measure.h"
int main(){
	struct DMD dmd;
	while(true){
	initDMD(&dmd);
	moveDMD(dmd);
	closeDMD(&dmd);
	printf("press any key to continue. Press q to quit\n");
	char a = getchar();
	if(a == 'q' || a == 'Q') return 0;
	}
	return 0;
}


