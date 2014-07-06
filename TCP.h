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

int convertTabtoInt(int[] , int );
char* convertTabtoString(char[], int );
char* convertHeaderToBuffer(header , int );
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
    char* data;
} header;

int IntToBinary(int i){
    int a[20];
	int j=0;
    while(i>0)
    {
		a[j]=i%2; 
		j++; 
		i=i/2;
    }
    return convertTabtoInt(a, 20);
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

int convertTabtoInt(int tab[], int taille){
    int result = 0, i;
    for(i = taille-1 ; i >= 0 ; i--){
        result *= 10;
        result += tab[i];
    }
    return result;
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
