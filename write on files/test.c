#include <stdio.h>
int main(){
int count = 0;
printf("how many do you want to enter: ");
scanf("%d", &count);
int val = 0;
int sum = 0;
int i = 0;
while(scanf("%d", &val) == 1 && i++ < count){
  sum += val;
  printf("%d ", val);
}
return 0;
}

