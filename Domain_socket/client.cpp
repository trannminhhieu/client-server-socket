#define SOCKET_NAME "/tmp/mysocket.sock"
#define BUF_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_un server_addr;
    char buffer[BUF_SIZE];
    int n;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_NAME);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }

    while (1)
    {
        bzero(buffer, BUF_SIZE);
        printf(">> ");
        fgets(buffer, BUF_SIZE, stdin);
        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
        std::string msg = "[Client] " + std::string(buffer);
        n = write(sockfd, msg.c_str(), BUF_SIZE);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
        bzero(buffer, BUF_SIZE);
        n = read(sockfd, buffer, BUF_SIZE);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("%s\n", buffer);
    }
    close(sockfd);
    return 0;
}