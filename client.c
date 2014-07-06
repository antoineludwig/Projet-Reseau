#include "TCP.h"

int main(int argc, char* argv[]){
//test entrées
	if(argc!=3){
		printf("Nombre d'arguments invalide\n");
		exit(-1);
	}
	/*else if(sizeof(argv[1])!=sizeof(int)){
		perror("Le numéro de port doit etre un entier\n");
		exit(-1);		
	}*/
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
        /*char cou[7];
		int b=42;
		if(send(socketd,&b,sizeof(int),0)<0){
			printf("errorcli\n");
        }*/
/*		sprintf(cou,"coucou\0");
		send(socketd,&cou,7*sizeof(char),0);*/
	//fin du test
	//construction du header TCP
		//data à envoyer
		char data[7];
        sprintf(data,"coucou\0");
        printf("Start header\n");
        header h;
        h.src_port=IntToBinary(ntohs(saddrCli.sin_port));
        h.dest_port=IntToBinary(htons(atoi(argv[2])));
		//premiere valeur =0, à chaque echange ajouter la taille des données envoyés (1ko->numseq=numseq+1024) (utilisé sizeof)	
		h.seq_num=IntToBinary(0);
		//ack =numseq+1
		h.ack=IntToBinary(BinaryToInt(h.seq_num)+1);
		//nombre de ligne(sur schéma header tcp) avant que les données commence
		//comme on fait tt le header tjrs 6
		h.data_offset=IntToBinary(6);
		//tjrs a 0		
		h.reserved=IntToBinary(0);
		//nombre de demande client avant acquitement (ack) du serveur (on va repondre tt de suite)
		h.window=IntToBinary(1);
		//Addition de tous le header(ramené à 16bit) (meme les data) puis complement à 1(xor 1111111111111111)
		h.checksum=calculCheksum(h.src_port&h.dest_port,h.seq_num,h.ack,h.data_offset&h.reserved&h.window,h.urgent,h.option,h.padding,h.data);
		h.urgent=IntToBinary(0);
		h.option=IntToBinary(0);
		h.padding=IntToBinary(0);
        h.data=convertTabtoString(data, 7);
		//www.frameip.com/entete-tcp/
        printf("Envoi au serveur : %s\n", data);
        convertHeaderToBuffer(h, 1024);
		if(send(socketd, &h, sizeof(header), 0)<0)
		{
			perror("send()");
			exit(EXIT_FAILURE);
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
