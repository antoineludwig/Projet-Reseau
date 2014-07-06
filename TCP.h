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

unsigned int convertTabtoInt(int[], int);

typedef struct {
	
    unsigned int src_port;
    unsigned int dest_port;
    unsigned int seq_num;
    unsigned int ack;
    unsigned int data_offset;
    unsigned int reserved;
    unsigned int window;
    unsigned int checksum;
    unsigned int urgent;
    unsigned int option;
    unsigned int padding;
    int data;
} header;

unsigned int IntToBinary(int i){
    unsigned int result = 0;
    int j=0;
    while(i>0)
    {
        result = result * 10 + i%2;
		j++; 
        i=i/2;
    }
    return result;
}

int BinaryToInt(unsigned int n){
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

char* completebinary(int taille, unsigned int binary){
    char* result = (char*)malloc(sizeof(char)*taille);
    int i;
    int tailleReel = sizeInt(binary);
    int tailleRestante = taille - tailleReel;

    for(i = taille-1 ; i >= tailleRestante ; i--){
        result[i] = ((binary %10)+48);
        binary /= 10;
    }

    for(i = tailleRestante-1 ; i >= 0 ; i--){
        result[i] = '0';
     }
    printf("%s\n", result);
    return result;
}

char* convertHeaderToBuffer(header h, int tailleBuffer){
    char* stringHeader = (char*)malloc(sizeof(char)*(6*32 + 1024));
    return "";
}
int sizeInt(int nombre){
    int taille = 0;
    while(nombre > 0){
        taille++;
        nombre /= 10;
    }
    return taille;
}
