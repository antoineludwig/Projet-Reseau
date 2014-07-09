#include "TCP.h"
#include <stdio.h>
#include <inttypes.h>

void checkParameters(int argc, char* argv[]);
void checkBindSocket(int socketd, struct sockaddr_in saddr);
void checkListenSocket(int socketd);
struct sockaddr_in initServerAdress(struct sockaddr_in saddr, int port);
int acceptClient(int idSockCli, int socketd, struct sockaddr_in saddrCli, socklen_t saddrCliLen);
void printClientInformation(struct sockaddr_in saddrCli);

int main(int argc, char* argv[]){
    checkParameters(argc, argv);

    printf("\nLancement du serveur\n");
    //création de la socket
    int socketd,idSockCli;
    struct sockaddr_in saddr, saddrCli;
    socklen_t saddrCliLen;
    pid_t pid;
    header h;
    socketd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socketd<0){
        printf("Erreur creation socket\n");
        exit(-1);
    }

    saddr = initServerAdress(saddr, atoi(argv[1]));

    checkBindSocket(socketd, saddr);

    checkListenSocket(socketd);
    //lecture socket
    while(1){

        //recup addr client
        saddrCliLen=sizeof(saddrCli);
        //acceptation connection client socket
        idSockCli = acceptClient(idSockCli, socketd, saddrCli, saddrCliLen);

        printClientInformation(saddrCli);
        //test reception
        pid=fork();

        if(pid==0){
            int rec=recv(idSockCli,&h,sizeof(header),0);

            if(rec<0){
                printf("err=%d\n",rec);
            }
            else{
                //printf("number=%d\n",h.data);
                h.ack = ntohs(htons(h.ack) + 1);
                printHeader(h);

                //TODO réponse du serveur au client

                /*if(send(socketd, &h, sizeof(h), 0) < 0){
                    printf("errorcli\n");
                }*/

            }
            printf("Erreur : %s\n",strerror(errno));
        }
        else if(pid==-1){
            exit(-1);
        }
        else{
            //Pere
        }
        //fin du test

        //envoi de la structure complète (pas champ par champ)
        int taille_envoi = sizeof(header);

        printf("Envoi d'un message de taille %d\n",taille_envoi);

        printf("Envoi de la taille du message au serveur\n");
        send(idSockCli,&h,taille_envoi,0);
    }

    return 0;
}

void checkParameters(int argc, char* argv[]){
    if(argc!=2){
        printf("Nombre d'arguments invalide\n");
        exit(-1);
    }
}

struct sockaddr_in initServerAdress(struct sockaddr_in saddr, int port){
    //remplis de zero
    bzero(&saddr,sizeof(saddr));
    //défini la famille
    saddr.sin_family=AF_INET;
    //defini le port
    saddr.sin_port=htons(port);
    //associe adresse
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    return saddr;
}

void checkBindSocket(int socketd, struct sockaddr_in saddr){
    if(bind(socketd,(const struct sockaddr *)&saddr,sizeof(saddr))<0){
        printf("Erreur bind socket\n");
        exit(-1);
    }
}

void checkListenSocket(int socketd){
    if(listen(socketd,SOMAXCONN)<0){
        printf("Erreur ecoute socket\n");
        exit(-1);
    }
}

int acceptClient(int idSockCli, int socketd, struct sockaddr_in saddrCli, socklen_t saddrCliLen){
    idSockCli = accept(socketd,(struct sockaddr *)&saddrCli,&saddrCliLen);
    if(idSockCli < 0){
        printf("Erreur creation socket\n");
        exit(-1);
    }
    return idSockCli;
}

void printClientInformation(struct sockaddr_in saddrCli){
    int port;
    char adresseIP[16];
    port=ntohs(saddrCli.sin_port);
    inet_ntop(AF_INET,&saddrCli.sin_addr,adresseIP,sizeof(adresseIP));
    printf("connexion de %s, port %d\n",adresseIP,port);
}
