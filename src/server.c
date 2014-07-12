#include "TCP.h"
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <setjmp.h>

sigjmp_buf contexteAlarme;

void gestionAlarme(int numsig){
	//signal(SIGALRM,SIG_IGN);
	printf("TIME OUT\n");
	//signal(SIGALRM,gestionAlarme);
	longjmp(contexteAlarme,1);
}

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
    int rec=0;
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
	signal(SIGALRM,gestionAlarme);
	alarm(5);
	if(setjmp(contexteAlarme)==1){
		close(idSockCli);
	}
	else{
            rec=recv(idSockCli,&h,sizeof(header),0);
	}
            if(rec<0){
                printf("err=%d\n",rec);
            }
            else{
		printf("RECEPTION SYN\n");
                if(htons(h.flags_of)!=2){//reception de SYN
                	printf("erreur : reception signal SYN\n");
			close(idSockCli);
                }
		else{   
			printf("ENVOI ACK+SYN\n");            	
			h.ack = ntohs(htons(h.seq)+1);
			h.flags_of = ntohs(18); //envoie ACK+SYN
			if(send(idSockCli, &h, sizeof(header), 0) < 0){
                		printf("erreur envoie signal SYN+ACK\n");
				close(idSockCli);
                	}
			printf("RECEPTION DATA\n");
			//reception des données
			alarm(5);
			if(setjmp(contexteAlarme)==1){
				close(idSockCli);
			}
			else{
				rec=recv(idSockCli,&h,sizeof(header),0);
			}
			printHeader(h);
			if(rec<0){
				printf("err=%d\n",rec);
				close(idSockCli);
			}
			else{
				printf("ENVOIE DATA REPONSE\n");
				//envoie de la réponse
				h.ack = ntohs(htons(h.seq)+1);
				h.data=ntohs(htons(h.data)+1);
				h.seq = h.seq+h.data;
				printHeader(h);
				if(send(idSockCli, &h, sizeof(h), 0) < 0){
                			printf("erreur envoie de la réponse\n");
					close(idSockCli);
                		}
				printf("RECEPTION FIN\n");
				//reception FIN
				alarm(5);
				if(setjmp(contexteAlarme)==1){
					close(idSockCli);
				}
				else{
					rec=recv(idSockCli,&h,sizeof(header),0);
				}
				if(rec<0){
					printf("err=%d\n",rec);
					close(idSockCli);
				}else{
					if(htons(h.flags_of)!=1){
						printf("erreur : signal FIN non recu\n");
						close(idSockCli);
					}
				}
			}
		}                

            }
        }
        else if(pid==-1){
            exit(-1);
        }
        else{
            //Pere
        }
        //fin du test
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
