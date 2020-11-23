#include"dmd.h"
int main(){
hid_device *handle = hid_open(0x0451, 0xc900, NULL);


if (!handle) {
	printf("unable to open device\n");
	handle = NULL;
	if(!DEBUG) return 1;
	}
changeMode(handle, 3);
int exposure[1];
exposure[0] = 10000000;
int dark_time[1]={0};
dark_time[0] = 0;
int trigger_in[1];
trigger_in[0] = 0;
int trigger_out[1];
trigger_out[0] = 1;
int images[1][1080][1920];

for(int i = 0; i<1080; i++){
	for(int j = 0; j<1920; j++){
		if(j<1000 && i <500) images[0][i][j]=0;
		else images[0][i][j]=1;
	}	
	
}
defSequence(handle,images,exposure,trigger_in,dark_time,trigger_out, 60,1);
startSequence(handle);

return 0;
}


