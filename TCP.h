#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
long convertTabtoInt(int[], int);
typedef struct {
	
    long src_port;
    long dest_port;
    long seq_num;
    long ack;
    long data_offset;
    long reserved;
    long window;
    long checksum;
    long urgent;
    long option;
    long padding;
    char* data;
} header;

long IntToBinary(int i){
    long result = 0;
    int j=0;
    while(i>0)
    {
        result = result * 10 + i%2;
		j++; 
        i=i/2;
    }
    return result;
}

int BinaryToInt(long n){
	int decimal=0, i=0, rem;
	while (n!=0)
   	{
        rem = n%10;
        n/=10;
        decimal += (int)rem*pow(2,i);
        ++i;
    }
    return decimal;
}

char* convertTabtoString(char tab[], int taille){
    char* result = (char*)malloc(sizeof(char)*taille);
    int i;
    for(i = 0 ; i < taille ; i++){
        result[i] = tab[i];
    }
    return result;
}

char* convertHeaderToBuffer(header h, int tailleBuffer){
    //TODO convert header to String
    return "";
}
