#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

#define KILOBYTE (1024 * 1024)

#define CHECK_RESULT_AND_RETURN(result) \
{ \
    if((result) == -1) \
    { \
        perror(NULL); \
        return false; \
    } \
}

bool run_client(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);

    CHECK_RESULT_AND_RETURN(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(options.host);
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    char payload[] = "GET / HTTP/1.1\r\nUser-Agent: curl/7.32.0\r\nHost: 192.168.0.107\r\nAccept: */*\r\n\r\n";

    int connectOk = connect(fSocket, (struct sockaddr*) &serverAddr, sizeof (struct sockaddr_in));
    CHECK_RESULT_AND_RETURN(connectOk);

    int payloadLength = strlen(payload);
    int bytesSent = send(fSocket, payload, payloadLength, 0);
    CHECK_RESULT_AND_RETURN(bytesSent);

    if (bytesSent != payloadLength)
    {
        printf("Sent %d out of %d", bytesSent, payloadLength);
        return false;
    }

    char buffer[KILOBYTE + 1];
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, KILOBYTE, 0);
    CHECK_RESULT_AND_RETURN(bytesRead);

    buffer[bytesRead] = 0;
    printf("%s", buffer);

    shutdown(fSocket, 2);
    close(fSocket);
    printf("Closed all and finished.");

    return true;
}

