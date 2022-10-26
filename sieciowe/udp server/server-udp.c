#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT 2020
#define MAXLINE 65607

bool ok(char *buf, int n)
{
    int i;
    for(i=0; i<n; i++)
        if((buf[i] < 48 || buf[i] > 57) && buf[i] != 13 && buf[i] != 10 && buf[i] != 32)
            return 0;
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

int main()
{
    int sockfd;
    char buf[MAXLINE];

    struct sockaddr_in seraddr, cliaddr;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    memset(&seraddr, 0, sizeof(seraddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = INADDR_ANY;
    seraddr.sin_port = htons(PORT);

    if(bind(sockfd, (const struct sockaddr*) &seraddr, sizeof(seraddr)) < 0)
    {
        perror("bind error");
        exit(2);
    }
    char *Error = "ERROR";
    int n;
    socklen_t len = sizeof(cliaddr);
    while(1)
    {
        n = recvfrom(sockfd, (char*) buf, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
        if(buf[0] == 10) break;
        buf[n] = '\0';
        if(n < 0)
        {
            perror("recvfrom error");
            exit(3);
        }
        if(!ok(buf, n)) 
        {
            if(sendto(sockfd, (char*) Error, strlen(Error), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr)) < 0)
            {
                perror("sendto error");
                exit(5);
            }
            continue;
        }
        int i, suma = 0;
        bool czy_overflow = 0, czy_liczba = 0;
        char *liczba = "";
        for(i=0; i<n+1; i++)
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

            // if((buf[i+1] == '\n' || buf[i+1] == '\0') && strlen(liczba) > 11 && suma != 0)
            // {
            //     czy_overflow = 1;
            //     break;
            // }
            // if((buf[i+1] == '\n' || buf[i+1] == '\0') && strlen(liczba) > 11 && suma == 0)
            // {
            //     czy_overflow = 1;
            //     break;
            // }
        }
        if(czy_overflow || !czy_liczba) 
        {
            if(sendto(sockfd, (char*) Error, strlen(Error), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr)) < 0)
            {
                perror("sendto error");
                exit(5);
            }
            continue;
        }
        char r[11];
        sprintf(r, "%i", suma);
        if(sendto(sockfd, (char*) r, strlen(r), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr)) < 0)
        {
            perror("sendto error");
            exit(5);
        }
    }
    return 0;
}