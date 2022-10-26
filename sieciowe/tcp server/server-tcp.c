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

#define PORT 2020
#define MAXLINE 1024

bool ok(char *buf, int n)
{
	if(buf[0] == 32 || buf[n-1] == 32 || buf[n-2] == 32)
		return 0;
    int i;
    for(i=0; i<n-1; i++)
	{
        if((buf[i] < 48 || buf[i] > 57) && buf[i] != 13 && buf[i] != 10 && buf[i] != 32)
            return 0;
		if(i != n-2 && (buf[i] == 32 && buf[i+1] == 32))
			return 0;
	}
    return 1;
}

char* append(char* array, char a)
{
    size_t len = strlen(array);

    char *ret = malloc(len+2);

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}

void sumator(int connfd)
{
	char buf[MAXLINE];
	int n;
	char *Error = "ERROR\r\n";
	while(1)
    {
		memset(&buf, 0, sizeof(buf));
		n = read(connfd, buf, sizeof(buf));
		if(n == 0)
		{
			return;
		}
		if(n < 0)
		{
			perror("read error");
			exit(5);
		}
		if(!ok(buf, n))	
		{
			if(write(connfd, Error, sizeof(Error)) < 0)
			{
				perror("write error");
				exit(6);
			}
            continue;
		}	
		buf[n] = '\0';	

		int i, suma = 0;
        bool czy_overflow = 0, czy_liczba = 0;
        char *liczba = "";
        for(i=0; i<n; i++)
        {
            if(buf[i] <= 57 && buf[i] >= 48)
            {
                liczba = append(liczba, buf[i]);
                czy_liczba = 1;
            }

            if((buf[i] == '\n' || buf[i] == 32 || buf[i] == '\0') && strlen(liczba) != 0)
            {
                if(strlen(liczba) <= 10)
                {
                    int k = atoi(liczba);
                    if(k < 0)
                    {
                        czy_overflow = 1;                        
                        break;
                    }
                    else suma += k;
                    if(suma < 0)
                    {
                        czy_overflow = 1;                        
                        break;
                    }
					liczba = "";
                }
                else
				{
					czy_overflow = 1;                        
					break;
				}
            }
        }
        if(czy_overflow || !czy_liczba) 
        {
            if(write(connfd, (char*) Error, strlen(Error)) < 0)
			{
				perror("write error");
				exit(6);
			}
            continue;
        }
        char r[13];
        sprintf(r, "%i", suma);
		strcat(r, "\r\n");
        if(write(connfd, r, strlen(r)) < 0)
		{
			perror("write error");
			exit(6);
		}
    }
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in seraddr, cliaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }
	memset(&seraddr, 0, sizeof(seraddr));

	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_port = htons(PORT);

	if(bind(sockfd, (const struct sockaddr*) &seraddr, sizeof(seraddr)) < 0) 
    {
		perror("bind error");
        exit(2);
	}

	if(listen(sockfd, 5) < 0) 
    {
		perror("listen error");
		exit(3);
	}
	socklen_t len = sizeof(cliaddr);

	connfd = accept(sockfd, (struct sockaddr*) &cliaddr, &len);
	if(connfd < 0) 
    {
		perror("accept error");
		exit(4);
	}

	sumator(connfd);

	close(sockfd);
	close(connfd);

	return 0;
}
