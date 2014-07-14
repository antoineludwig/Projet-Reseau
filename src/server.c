#include "TCP.h"
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <setjmp.h>

sigjmp_buf contexteAlarme;

void gestionAlarme(int numsig);
void checkParameters(int argc, char* argv[]);
void checkBindSocket(int socketd, struct sockaddr_in saddr);
void checkListenSocket(int socketd);
struct sockaddr_in initServerAdress(struct sockaddr_in saddr, int port);
void acceptClient(int* idSockCli, int socketd, struct sockaddr_in saddrCli, socklen_t saddrCliLen);
void printClientInformation(struct sockaddr_in saddrCli);
int reception(int idSockCli, header h, int* rec, int closeIDSockCli);
void checkSignalFin(header h, int idSockCli);
void mySend(int idSockCli, header h, char* message);

int main(int argc, char* argv[])
{
    checkParameters(argc, argv);

    printf("\nLancement du serveur\n");
    //création de la socket
    int socketd,idSockCli;
    struct sockaddr_in saddr, saddrCli;
    socklen_t saddrCliLen;
    pid_t pid;
    header h;
    int rec=0;
    socketd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socketd<0)
    {
        printf("Erreur creation socket\n");
        exit(-1);
    }

    saddr = initServerAdress(saddr, atoi(argv[1]));

    checkBindSocket(socketd, saddr);

    checkListenSocket(socketd);
    //lecture socket
    while(1)
    {

        //recup addr client
        saddrCliLen=sizeof(saddrCli);
        //acceptation connection client socket
        acceptClient(&idSockCli, socketd, saddrCli, saddrCliLen);

        printClientInformation(saddrCli);
        //test reception
        pid=fork();
        if(pid==0)
        {
            signal(SIGALRM,gestionAlarme);
            alarm(5);
            if(reception(idSockCli, h, &rec, 0) != -1)
            {
                printf("RECEPTION SYN\n");
                if(htons(h.flags_of)!=2) //reception de SYN
                {
                    printf("erreur : reception signal SYN\n");
                    close(idSockCli);
                }
                else
                {
                    printf("ENVOI ACK+SYN\n");
                    h.ack = ntohs(htons(h.seq)+1);
                    h.flags_of = ntohs(18); //envoie ACK+SYN
                    mySend(idSockCli, h, "Erreur envoie signal SYN+ACK");

                    printf("RECEPTION DATA\n");
                    //reception des données

                    if(reception(idSockCli, h, &rec, 1) != -1)
                    {
                        printHeader(h);
                        printf("ENVOIE DATA REPONSE\n");
                        //envoie de la réponse
                        h.ack = ntohs(htons(h.seq)+1);
                        h.data=ntohs(htons(h.data)+1);
                        h.seq = h.seq+h.data;
                        printHeader(h);
                        mySend(idSockCli, h, "erreur envoie de la réponse");
                        printf("RECEPTION FIN\n");
                        //reception FIN
                        if(reception(idSockCli, h, &rec, 1) != -1)
                        {
                            checkSignalFin(h, idSockCli);
                        }
                    }
                }

            }
        }
        else if(pid==-1)
        {
            exit(-1);
        }
    }

    return 0;
}

void checkParameters(int argc, char* argv[])
{
    if(argc!=2)
    {
        printf("Nombre d'arguments invalide\n");
        exit(-1);
    }
}

struct sockaddr_in initServerAdress(struct sockaddr_in saddr, int port)
{
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

void checkBindSocket(int socketd, struct sockaddr_in saddr)
{
    if(bind(socketd,(const struct sockaddr *)&saddr,sizeof(saddr))<0)
    {
        printf("Erreur bind socket\n");
        exit(-1);
    }
}

void checkListenSocket(int socketd)
{
    if(listen(socketd,SOMAXCONN)<0)
    {
        printf("Erreur ecoute socket\n");
        exit(-1);
    }
}

void acceptClient(int* idSockCli, int socketd, struct sockaddr_in saddrCli, socklen_t saddrCliLen)
{
    *idSockCli = accept(socketd,(struct sockaddr *)&saddrCli,&saddrCliLen);
    if(*idSockCli < 0)
    {
        printf("Erreur creation socket\n");
        exit(-1);
    }
}

void printClientInformation(struct sockaddr_in saddrCli)
{
    int port;
    char adresseIP[16];
    port=ntohs(saddrCli.sin_port);
    inet_ntop(AF_INET,&saddrCli.sin_addr,adresseIP,sizeof(adresseIP));
    printf("connexion de %s, port %d\n",adresseIP,port);
}

void gestionAlarme(int numsig)
{
    //signal(SIGALRM,SIG_IGN);
    printf("TIME OUT\n");
    //signal(SIGALRM,gestionAlarme);
    longjmp(contexteAlarme,1);
}

int reception(int idSockCli, header h, int* rec, int closeIDSockCli)
{
    alarm(5);
    if(setjmp(contexteAlarme)==1)
    {
        close(idSockCli);
    }
    else
    {
        *rec=recv(idSockCli,&h,sizeof(header),0);
    }
    if(*rec < 0)
    {
        printf("err=%d\n", *rec);
        if(closeIDSockCli == 1)
        {
            close(idSockCli);
        }
        return -1;
    }

    return 0;
}

void checkSignalFin(header h, int idSockCli)
{
    if(htons(h.flags_of)!=1)
    {
        printf("erreur : signal FIN non recu\n");
        close(idSockCli);
    }
}

void mySend(int idSockCli, header h, char* message)
{
    if(send(idSockCli, &h, sizeof(h), 0) < 0)
    {
        printf("%s\n", message);
        close(idSockCli);
    }
}
