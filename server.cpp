#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clientlen;
    char buffer[256];
    struct sockaddr_in server_addr, client_addr;
    int n;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    portno = 8080;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }
    printf("Server address is %s\n", inet_ntoa(server_addr.sin_addr));
    printf("Server Waiting for connection on port %d\n", portno);

    listen(sockfd, 5);
    clientlen = sizeof(client_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        printf("Server accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));

        while (1)
        {
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255);
            if (n < 0)
            {
                perror("ERROR reading from socket");
                exit(1);
            }
            else if (n == 0)
            {
                printf("Client disconnected\n");
                break;
            }
            printf("[Client]: %s", buffer);

            n = write(newsockfd, "Message received\n", 18);
            if (n < 0)
            {
                perror("ERROR writing to socket");
                exit(1);
            }
        }
        close(newsockfd);
        printf("Waiting for a new connection...\n");
    }

    close(sockfd);
    return 0;
}