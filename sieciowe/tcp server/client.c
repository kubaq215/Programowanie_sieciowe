#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX 1024
#define PORT 2020
#define SA struct sockaddr


void func(int sockfd)
{
	char buff[MAX];
	int n;
	while(1)
    {
		memset(&buff, 0, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		memset(&buff, 0, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) 
        {
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) 
    {
		printf("socket creation failed...\n");
		exit(0);
	}
	// else
		// printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    {
		printf("connection with the server failed...\n");
		exit(0);
	}
	// else
		// printf("connected to the server..\n");

	char buf1[] = "2 2";
	char buf2[] = "3 3";
	char buf3[] = "4 4";
	char buff[20];
	// printf("%s\n", buf);
	write(sockfd, buf1, sizeof(buf1));
	read(sockfd, buff, sizeof(buff));
	printf("%s", buff);
	memset(&buff, 0, sizeof(buff));

	write(sockfd, buf2, sizeof(buf2));
	read(sockfd, buff, sizeof(buff));
	printf("%s", buff);
	memset(&buff, 0, sizeof(buff));

	write(sockfd, buf3, sizeof(buf3));
	read(sockfd, buff, sizeof(buff));
	printf("%s", buff);
	memset(&buff, 0, sizeof(buff));
	// close the socket
	close(sockfd);
}
