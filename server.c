#include "TCP.h"

int main(int argc, char* argv[]){
//test entrées
	if(argc!=2){
		printf("Nombre d'arguments invalide\n");
		exit(-1);
	}
	/*else if(sizeof(argv[1])!=sizeof(int)){
		perror("Le numéro de port doit etre un entier\n");
		exit(-1);		
	}*/
	else{
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
		//remplis de zero
		bzero(&saddr,sizeof(saddr));
		//défini la famille
		saddr.sin_family=AF_INET;
		//defini le port
		saddr.sin_port=htons(atoi(argv[1]));	
		//associe adresse
		saddr.sin_addr.s_addr=htonl(INADDR_ANY);
	//handler
	//bind socket
		if(bind(socketd,(const struct sockaddr *)&saddr,sizeof(saddr))<0){
			printf("Erreur bind socket\n");
			exit(-1);	
		}
	//listen socket
		if(listen(socketd,SOMAXCONN)<0){
			printf("Erreur ecoute socket\n");
			exit(-1);	
		}
	//lecture socket	
		while(1){
		
			//recup addr client & acceptation connection client socket
			saddrCliLen=sizeof(saddrCli);
			idSockCli=accept(socketd,(struct sockaddr *)&saddrCli,&saddrCliLen);
			if(idSockCli<0){
				printf("Erreur creation socket\n");
				exit(-1);	
			}
		
			//affichage test client connecte		
			int port;
			char adresseIP[16];
			port=ntohs(saddrCli.sin_port); 
			inet_ntop(AF_INET,&saddrCli.sin_addr,adresseIP,sizeof(adresseIP)); 
			printf("connexion de %s, port %d\n",adresseIP,port);
		//test reception
			pid=fork();
			int b=0;
			int rec=0;
			if(pid==0){
				rec=recv(idSockCli,&b,sizeof(int),0);
				if(rec<0){
					printf("err=%d\n",rec);
				}
				else
					printf("b=%d\n",b);
				printf("%s\n",strerror(errno));
			}
			else if(pid==-1){
				exit(-1);
			}
			else{
				printf("Pere\n");
			}
		//fin du test

//envoi de la structure complète (pas champ par champ)
		int taille_envoi = sizeof(header);

		printf("Envoi d'un message de taille %d\n",taille_envoi);

		printf("Envoi de la taille du message au serveur\n");
        send(idSockCli,&taille_envoi,taille_envoi,0);
        /*int retour = send(idSockCli,&header,taille_envoi,0);
		printf("Envoi du message\n");
		//envoi sur la socket de la structure, avec un pointeur sur la struct définie et la taille de la structure (obtenue par 		le sizeof ligne 112)
        if(retour == -1)
		{
			perror("send()");
			exit(EXIT_FAILURE);
        }*/
		
		}
	}
	return 0;
}
