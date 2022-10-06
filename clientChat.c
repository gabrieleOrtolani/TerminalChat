#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#define MAX 80
#define PORT 2222
#define SA struct sockaddr
void func(void* param)
{
	char buff[MAX];
	int n;
	int j;
        int sockfd = (int) param;

chat:
	while(1) {

		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("server >> %s\n",buff);
		n=0;
		

		if((strcmp("insert the message to send",buff)==0)){
			printf("ti chiedo il messaggio\n");
			bzero(buff,MAX);
			printf("you>>: ");
			j=0;

			while((buff[j++] = getchar()) != '\n');
			
			write(sockfd,buff,sizeof(buff));
			goto chat;
			/*
			bzero(buff,MAX);
			read(sockfd,buff,sizeof(buff));
			*/
		}
			
		bzero(buff,MAX);
		printf("you >>: ");
		
		/*
		while ((buff[n++] = getchar()) != '\n')
			;
		*/
		scanf("%s",buff);
		write(sockfd, buff, sizeof(buff));
		
		
		
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;

		}
	}

}
int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
        pthread_t tid;


	//*creo il socket*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	/*riempio la struttura*/
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	/*connetto il client con il server*/ 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
		!= 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else{
		printf("connected to the server..\n");
                
        }

	/*inizio la chat*/
	func(sockfd);


	/*chiudo il socket*/
	
}
