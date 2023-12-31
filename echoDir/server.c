#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h> // memset
#include <errno.h>

#define BUF_SIZE 1000

//code from https://mohsensy.github.io/programming/2019/09/25/echo-server-and-client-using-sockets-in-c.html

int main(int argc, char const *argv[]) {


    int serverFd, clientFd;
    struct sockaddr_in server, client;
    unsigned int len;
    int port = 1234;
    char buf[1024];
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    //use tcp/IPv4
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("Cannot create socket");
        exit(1);
    }
    server.sin_family = AF_INET;            //use IPv4 addresses
    server.sin_addr.s_addr = htonl(INADDR_ANY);    //use any ip address on our host
    printf("Port: %d\n", port);
    server.sin_port = htons(port);          //converts from host byte to network-byte order
    len = sizeof(server);

    if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
        perror("Cannot bind sokcet");
        exit(2);
    }
    if (listen(serverFd, 10) < 0) {
        perror("Listen error");
        exit(3);
    }

    while (1) {
        len = sizeof(client);
        printf("waiting for clients\n");
        if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept error");
            exit(1);
        }

        char *client_ip = inet_ntoa(client.sin_addr);
        printf("Accepted new connection from a client %s:%d\n\n", client_ip, ntohs(client.sin_port));
        memset(buf, 0, sizeof(buf));

//        int size = read(clientFd, buf, sizeof(buf));
//        if ( size < 0 ) {
//            perror("read error");
//            exit(5);
//        }
//        printf("received %s from client\n", buf);
//        if (write(clientFd, buf, size) < 0) {
//            perror("write error");
//            exit(6);
//        }

        char buf[BUF_SIZE];
        ssize_t numRead;
        while ((numRead = read(clientFd, buf, BUF_SIZE)) > 0) {
            write(1, buf, numRead); //write to stdout
            //write to client what we read
            if (write(clientFd, buf, numRead) != numRead) {
                perror("write");
                exit(EXIT_FAILURE);
            }
//            printf("numRead: %zu\n", numRead);
        }

//        printf("%zu\n", write(clientFd, "Service Complete", 100));
        close(clientFd);
    }
    close(serverFd);
    return 0;
}