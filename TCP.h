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
  uint16_t src_port;
  uint16_t dst_port;
  uint32_t seq;
  uint32_t ack;
  uint16_t  flags_of;//offset+flags
  uint16_t window_size;
  uint16_t checksum;
  uint16_t urgent_p;
  uint16_t option;
  uint16_t padding;

  uint32_t data;
} header;

void printHeader(header h){
    printf("#### AFFICHAGE DU HEADER ####\n");
    printf("src_port = %d\n", htons(h.src_port));
    printf("dst_port = %d\n", htons(h.dst_port));
    printf("seq = %d\n", htons(h.seq));
    printf("ack = %d\n", htons(h.ack));
    //printf("data_offset = %d\n", htons(h.data_offset));
    printf("flags_of = %d\n", htons(h.flags_of));
    printf("window_size = %d\n", htons(h.window_size));
    printf("checksum = %d\n", htons(h.checksum));
    printf("urgent_p = %d\n", htons(h.urgent_p));
    printf("option = %d\n", htons(h.option));
    printf("padding = %d\n", htons(h.padding));
    printf("data = %d\n", htons(h.data));
    printf("#############################\n");
}

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

