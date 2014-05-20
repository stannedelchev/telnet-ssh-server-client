#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv) {

    int fSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(fSocket == -1) {
        printf("%d", errno);
        return EXIT_FAILURE;
    }

    
    struct sockaddr_in serverAddr;
    
    serverAddr.sin_family = AF_INET;          // host byte order
    serverAddr.sin_port = htons(80);   // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.107");
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    char payload[] = "GET / HTTP/1.1\r\nUser-Agent: curl/7.32.0\r\nHost: 192.168.0.107\r\nAccept: */*\r\n\r\n";

    if(connect(fSocket, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("%d", errno);
        perror(NULL);
        return EXIT_FAILURE;
    }
    
    int payloadLength = strlen(payload);
    int bytesSent = send(fSocket, payload, payloadLength, 0);
    if(bytesSent == -1)
    {
        perror(NULL);
        return EXIT_FAILURE;
    }
    
    if(bytesSent != payloadLength)
    {
        printf("Sent %d out of %d", bytesSent, payloadLength);
        return EXIT_FAILURE;
    }
    
    char buffer[1024 * 1024 + 1];
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, 1024 * 1024, 0);
    if(bytesRead == -1)
    {
        printf("%d", errno);
        perror(NULL);
        return EXIT_FAILURE;
    }
    
    buffer[bytesRead] = 0;
    printf("%s", buffer);
    
    shutdown(fSocket, 2);
    close(fSocket);
    printf("Closed all and finished.");
    return (EXIT_SUCCESS);
}

