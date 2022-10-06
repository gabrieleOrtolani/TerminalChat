#include <stdio.h>
#include <netdb.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define MAX 80
#define PORT 2222
#define SA struct sockaddr

FILE *ret[10];

int check_user(char* user){
	   struct dirent **namelist;
       int n;
       n = scandir("./", &namelist, NULL, alphasort);
       if (n < 0)
           perror("scandir");
       else {
           while (n--) {
               if(strcmp(user, namelist[n]->d_name)==0){
					free(namelist[n]);
					printf("I am looking for the user in the database...\n");
					return 1;
			   }
               free(namelist[n]);
           }
           free(namelist);
		   return 0;
       }
}
void chat_func(int connfd, char *user,char * recipient){
	char date[256];	
	char buff[MAX];
	time_t t = time(NULL);
    struct tm tm = *localtime(&t);
	char cache[MAX];


	sprintf(cache,"./%s/message",recipient);
	mkdir(cache,0777);

	bzero(date,256);
	char request[MAX];
	char object[MAX];

	bzero(buff,MAX);
	write(connfd,"insert the object",sizeof("insert the object")); //1
new_message:
	bzero(buff,MAX);
	read(connfd,buff,sizeof(buff));	//2
	strcpy(object,buff); 
	bzero(buff,MAX);
	write(connfd,"insert the message to send",sizeof("insert the message to send"));
	bzero(buff,MAX);
	read(connfd,buff,sizeof(buff));
	sprintf(date,"%02d-%02d-%d-%02d-%02d", tm.tm_hour+6, tm.tm_min, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
	sprintf(request,"%s/%s-%s-%s.txt",cache,user,date,object,recipient);
	printf("%s\n",request);
	fflush(stdout);
	ret[connfd] = fopen(request,"w+");
	
	if (strncmp("exit", buff, 4) == 0) {
		perror("Chat Exit...\n");
		exit(1);
	}
	printf("%s\n",buff);
	fprintf(ret[connfd],"%s\n%s\n",date,buff);
	fflush(ret[connfd]);
	printf("1 message was stored\n");
	bzero(buff,MAX);
	write(connfd,"Message stored, insert the object for other message or exit",sizeof("Message stored, insert the object for other message or exit"));
	goto new_message;
		
	}
	
	


int check_psw(char *psw1,char *user,int curr){
	char buffer[MAX];
	char request[MAX];
	sprintf(buffer,"%s/psw1.txt",user);
	ret[curr]= fopen(buffer,"r");
	
	int err = fgets(request,MAX,ret[curr]);

	request[strlen(request)-1]='\0';
	printf("psw1:%s-len:%d\n",psw1,strlen(psw1));
	printf("request:%s-len:%d\n",request,strlen(request));
	if(strcmp(psw1,request)==0){
		printf("%s has logged in...\n",user);
		return 1;
	}else{
	
	printf("%s entered a wrong password\n",user);
	return 0;
	}
}
/*funzione di login*/
void* func(void* param)
{
	char buff[MAX];
	int n;
    int connfd  = (int)param;
	char name[MAX];
	// infinite loop for chat
	write(connfd, "inserisci nome utente:",sizeof("inserisci nome utente:"));
	read(connfd, buff, sizeof(buff));
check_user1:
	
	snprintf(name,"%s",buff);
	if(check_user(name)==1){
		
		bzero(buff,MAX);
		write(connfd,"utente trovato, inserire password: ", sizeof("utente trovato, inserire password: "));
		bzero(buff,MAX);
redo:
		read(connfd,buff,sizeof(buff));
		if(check_psw(buff,name,connfd)==1){
			bzero(buff,MAX);
requestACT:
			write(connfd,"password corretta,cosa vuoi fare?", sizeof("password corretta,cosa vuoi fare?"));				
	
			bzero(buff,MAX);
			read(connfd,buff,sizeof(buff));
				

			if(strcmp("message",buff)==0){
redo_adddresse:
				write(connfd,"enter the addresse of the message",sizeof("enter the addressse of the message"));
				bzero(buff,MAX);
				read(connfd,buff,sizeof(buff));
				if(check_user(buff)==1){
					chat_func(connfd,name,buff);
				}
				goto redo_adddresse;
			}
			if(strcmp("segreteri",buff)==0){
				//segreteria_func(connfd,name);
				exit (1);
			}
			if(strcmp("elimina",buff)==0){
				//elimina_func(connfd,name);
				exit (1);
			}
		}
		/*se scrivo exit esce il server*/
		if (strncmp("exit", buff, 4) == 0) {
			perror("Server Exit...\n");
			exit(EXIT_FAILURE);
		}
		if(check_psw(buff,name,connfd)==0){
			write(connfd,"password errata, riprova", sizeof("password errata, riprova"));
			goto redo;
		}
	
	}
	else{
		mkdir(name,0777);
		char namefile[MAX];
		sprintf(namefile,"./%s/psw1.txt",name);
		ret[connfd] = fopen(namefile,"w+");
		if(ret==-1){
			perror("opening file");
			exit(EXIT_FAILURE);
		}
		bzero(buff,MAX);
		write(connfd,"utente creato, inserire nuova password: ", sizeof("utente creato, inserire nuova password: "));
		
		read(connfd,buff,sizeof(buff));
		char psw[MAX];
		strcpy(psw,buff);
		bzero(buff,sizeof(char)*strlen(buff));
		write(connfd,"utente creato, inserire per confrontare password: ", sizeof("utente creato, inserire per confrontare password: "));	
		read(connfd,buff,sizeof(buff));
confronta:
		if(strcmp(buff,psw)==0){
			printf("buff:%s\n",buff);
			fprintf(ret[connfd],"%s\n",psw);
			fflush(ret[connfd]);
			goto requestACT;
		}
		write(connfd,"password errata, riprova", sizeof("password errata, riprova"));	
		bzero(buff,MAX);
		read(connfd,buff,sizeof(buff));
		goto confronta;


	}

	pthread_exit(NULL);
}

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
    pthread_t tid;


	chdir("./DATA");

	/*creo il socket*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&(int){1}, sizeof(int));

	/*riempio la struttura*/
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	/*associo il socket con l'ind IP*/
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	/*il server si mette in ascolto*/
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	/*accetto i dati dal client e vedo se va a buon fine*/

	for(;;){
		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server accept failed...\n");
			exit(0);
		}
		else{
			printf("server accept the client...\n");
        	pthread_create(&tid,NULL,func,(void*)connfd);
    	}
		pthread_join(tid, NULL);
	}
	/*chiamo la chat*/
	/*func(connfd);*/
	
	/*chiudo il socket*/
	close(sockfd);
}
