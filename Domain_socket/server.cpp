#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <string>

#define SOCKET_NAME "/tmp/mysocket.sock"
#define BUF_SIZE 256

void cleanup_and_exit(int sockfd, const std::string &msg) {
    perror(msg.c_str());
    close(sockfd);
    unlink(SOCKET_NAME);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    socklen_t clientlen;
    char buffer[BUF_SIZE];
    struct sockaddr_un server_addr, client_addr;
    int n;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cleanup_and_exit(sockfd, "ERROR opening socket");
    }

    unlink(SOCKET_NAME);
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cleanup_and_exit(sockfd, "ERROR on binding");
    }

    printf("Server address is %s\n", server_addr.sun_path);
    printf("Server Waiting for connection\n");

    if (listen(sockfd, 5) < 0) {
        cleanup_and_exit(sockfd, "ERROR on listen");
    }

    clientlen = sizeof(client_addr);

    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen);
        if (newsockfd < 0) {
            cleanup_and_exit(sockfd, "ERROR on accept");
        }

        while (true) {
            memset(buffer, 0, BUF_SIZE);
            n = read(newsockfd, buffer, BUF_SIZE);
            if (n < 0) {
                perror("ERROR reading from socket");
                close(newsockfd);
                break;
            } else if (n == 0) {
                printf("Client disconnected\n");
                close(newsockfd);
                break;
            }
            printf("%s", buffer);

            std::string msg = "[Server] Message received";
            n = write(newsockfd, msg.c_str(), msg.length());
            if (n < 0) {
                perror("ERROR writing to socket");
                close(newsockfd);
                break;
            }
        }

        std::cout << "Waiting for new connection..." << std::endl;

        if (std::cin.peek() != EOF) {
            char command = std::cin.get();
            if (command == 'q' || command == 'Q') {
                break;
            }
        }
    }

    close(sockfd);
    unlink(SOCKET_NAME);
    return 0;
}
