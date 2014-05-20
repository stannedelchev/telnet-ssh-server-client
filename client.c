#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

bool run_client(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (fSocket == -1)
    {
        printf("%d", errno);
        return false;
    }

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(options.host);
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    char payload[] = "GET / HTTP/1.1\r\nUser-Agent: curl/7.32.0\r\nHost: 192.168.0.107\r\nAccept: */*\r\n\r\n";

    if (connect(fSocket, (struct sockaddr*) &serverAddr, sizeof (struct sockaddr_in)) == -1)
    {
        printf("%d", errno);
        perror(NULL);
        return false;
    }

    int payloadLength = strlen(payload);
    int bytesSent = send(fSocket, payload, payloadLength, 0);
    if (bytesSent == -1)
    {
        perror(NULL);
        return false;
    }

    if (bytesSent != payloadLength)
    {
        printf("Sent %d out of %d", bytesSent, payloadLength);
        return false;
    }

    char buffer[1024 * 1024 + 1];
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, 1024 * 1024, 0);
    if (bytesRead == -1)
    {
        printf("%d", errno);
        perror(NULL);
        return false;
    }

    buffer[bytesRead] = 0;
    printf("%s", buffer);

    shutdown(fSocket, 2);
    close(fSocket);
    printf("Closed all and finished.");

    return true;
}

