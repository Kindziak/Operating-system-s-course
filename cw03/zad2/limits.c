#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv){
	char tmp[5000001];
  int i;
  for(i=0 ; i<5000001 ; i++) tmp[i]='s';
  for( ; i>1 ; i--) if(tmp[1]==tmp[i-1]) tmp[i]=tmp[i-1];
	while(1){}
  return 0;
}
