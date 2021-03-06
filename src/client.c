#include "TCP.h"
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <setjmp.h>

sigjmp_buf contexteAlarme;

void checkParameters(int argc, char* argv[]);
void gestionAlarme(int numsig);
int calculCheksum(int port, int seq, int ack,int dataoffReservedWindow, int urg, int option, int padding, int Data);


int main(int argc, char* argv[])
{
    checkParameters(argc, argv);

    printf("Lancement client\n");

    int socketd;
    struct sockaddr_in saddrCli, saddrServ;
    struct hostent* hid;
    printf("Connexion serveur %s sur le port %s\n",argv[1],argv[2]);

    //lancement socket
    socketd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socketd<0)
    {
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
    if(bind(socketd,(const struct sockaddr *)&saddrServ,sizeof(saddrServ))<0)
    {
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
    if(connect(socketd,(struct sockaddr *)&saddrServ,sizeof(saddrServ))<0)
    {
        printf("Erreur de connexion\n");
        exit(-1);
    }

    signal(SIGALRM,gestionAlarme);

    //INITIALISATION DE LA CONNECTION
    //***envoie de SYN=1***
    header h;
    h.src_port = ntohs(saddrCli.sin_port);
    h.dst_port = ntohs(atoi(argv[2]));
    h.seq = ntohs(0);
    h.ack = ntohs(h.seq)+ntohs(1);
    h.flags_of = ntohs(2);
    h.window_size = ntohs(1);
    h.checksum = calculCheksum(h.src_port&h.dst_port,h.seq,h.ack,h.flags_of&h.window_size,h.urgent_p,h.option,h.padding,h.data);
    h.urgent_p = ntohs(0);
    h.option = ntohs(0);
    h.padding = ntohs(0);
    h.data= ntohs(0);
    printHeader(h);
    printf("ENVOI SYN\n");
    if(send(socketd, &h, sizeof(h), 0) < 0)
    {
        printf("errorcli\n");
    }
    //***reception de SYN+ACK=1***
    printf("RECEPTION SYN=1 et ACK=1 passage 1\n");
    int rec=0;
    alarm(5);
    if(setjmp(contexteAlarme)==1)
    {
        close(socketd);
    }
    else
    {
        rec=recv(socketd,&h,sizeof(header),0);
    }
    alarm(0);
    if(rec<0)
    {
        printf("err=%d\n",rec);
    }
    else
    {
        printf("RECEPTION SYN=1 et ACK=1 passage 2\n");
        if(htons(h.flags_of)!=18)
        {
            //reception de SYN+ACK
            printf("Erreur reception SYN+ACK\n");
            close(socketd);
        }
        else
        {
            printf("ENVOI DATA\n");
            h.ack = ntohs(htons(h.seq)+1);
            h.flags_of = ntohs(16); //envoie ACK
            h.data = ntohs(42); //envoie de données
            h.seq = h.seq+h.data;
            printHeader(h);
            //***envoie des données avec SYN=0***
            if(send(socketd, &h, sizeof(h), 0) < 0)
            {
                printf("erreur envoi données\n");
                close(socketd);
            }
            printf("RECEPTION REPONSE DATA\n");
            //***reception de la réponse***
            alarm(5);
            if(setjmp(contexteAlarme)==1)
            {
                close(socketd);
            }
            else
            {
                rec=recv(socketd,&h,sizeof(header),0);
            }
            if(rec<0)
            {
                printf("err=%d\n",rec);
            }
            else
            {
                printf("données reponse=%d\n",htons(h.data));
                printHeader(h);
                h.data = ntohs(0);
                h.ack = ntohs(htons(h.seq)+1);
                h.seq = h.seq+h.data;
                h.flags_of = ntohs(1);
                printf("ENVOI FIN\n");
                //***envoie de FIN***
                if(send(socketd, &h, sizeof(h), 0) < 0)
                {
                    printf("erreur envoi FIN\n");
                    close(socketd);
                }
            }

        }
    }

    printf("Envoi de la requête terminé\n");




    return 0;
}

void checkParameters(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("Nombre d'arguments invalide\n");
        exit(-1);
    }
}

int calculCheksum(int port, int seq, int ack,int dataoffReservedWindow, int urg, int option, int padding, int Data)
{
    int check=0;
    check=port&seq&ack&dataoffReservedWindow&urg&option&padding&Data;
    check=check^1111111111111111;
    return check;
}
void gestionAlarme(int numsig)
{
    //signal(SIGALRM,SIG_IGN);
    printf("TIME OUT\n");
    //signal(SIGALRM,gestionAlarme);
    longjmp(contexteAlarme,1);
}
