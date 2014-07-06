#include "TCP.h"

int main(int argc, char* argv[]){
//test entrées
    if(argc!=3){
        printf("Nombre d'arguments invalide\n");
        exit(-1);
    }
    else{
        printf("Lancement client\n");

        int socketd;
        struct sockaddr_in saddrCli, saddrServ;
        struct hostent* hid;

        printf("Connexion serveur %s sur le port %s\n",argv[1],argv[2]);

        //lancement socket
        socketd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(socketd<0){
            printf("Erreur creation socket\n");
            exit(-1);
        }
        //remplis de zero
        bzero(&saddrCli,sizeof(saddrCli));
        bzero(&saddrServ,sizeof(saddrServ));
        //défini la famille
        saddrCli.sin_family=AF_INET;
        //defini le port
        saddrCli.sin_port=htons(atoi(argv[2]));
        //associe adresse
        saddrCli.sin_addr.s_addr=htonl(INADDR_ANY);
    //bind socket
        if(bind(socketd,(const struct sockaddr *)&saddrServ,sizeof(saddrServ))<0){
            printf("Erreur bind socket\n");
            exit(-1);
        }
        //défini la famille
        saddrServ.sin_family=AF_INET;
        //defini le port
        saddrServ.sin_port=htons(atoi(argv[2]));
        //recup IP au bon format
        hid=gethostbyname(argv[1]);
        //copie l'adresse
        bcopy(hid->h_addr,&(saddrServ.sin_addr.s_addr),hid->h_length);

    //connexion à la socket
        if(connect(socketd,(struct sockaddr *)&saddrServ,sizeof(saddrServ))<0){
            printf("Erreur de connexion\n");
            exit(-1);
        }

    //test envoie de données
        int b=42;
        if(send(socketd,&b,sizeof(int),0)<0){
            printf("errorcli\n");
        }

        printf("Envoi de la requête terminé\n");



    }
    return 0;
}
int calculCheksum(int port, int seq, int ack,int dataoffReservedWindow, int urg, int option, int padding, int Data){
    int check=0;
    check=port&seq&ack&dataoffReservedWindow&urg&option&padding&Data;
    check=check^1111111111111111;
    return check;
}
