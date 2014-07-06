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

typedef struct {
	
	int src_port;
	int dest_port;
	int seq_num;
	int ack;
	int data_offset;
	int reserved;
	int window;
	int checksum;
	int urgent;
	int option;
	int padding;
	int data;
} header;

int* IntToBinary(int i){
    int* a = (int*)malloc(sizeof(int)*20);
	int j=0;
    while(i>0)
    {
		a[j]=i%2; 
		j++; 
		i=i/2;
    }
    return a;
}

int BinaryToInt(int n){
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
