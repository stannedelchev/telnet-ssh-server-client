#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

#define KILOBYTE (1024 * 1024)

#define CHECK_RESULT_AND_EXIT(result) \
{ \
    if((result) == -1) \
    { \
        perror(NULL); \
        exit(EXIT_FAILURE); \
    } \
}


void sendMessage(int fSocket, char* message);
char* receiveMessage(int fSocket);

bool run_client(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);

    CHECK_RESULT_AND_EXIT(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(options.host);
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    int connectOk = connect(fSocket, (struct sockaddr*) &serverAddr, sizeof (struct sockaddr_in));
    CHECK_RESULT_AND_EXIT(connectOk);

    const int MESSAGE_LENGTH = 256;
    char message[MESSAGE_LENGTH];
    while (fgets(message, MESSAGE_LENGTH, stdin))
    {
        printf("%s", message);
        if (strcmp(message, "\n") == 0 || strcmp(message, "\r\n") == 0)
        {
            break;
        }
        
        sendMessage(fSocket, message);
        char* serverMessage = receiveMessage(fSocket);
        printf("%s", serverMessage);
        free(serverMessage);
    }

    shutdown(fSocket, 2);
    close(fSocket);
    printf("Closed all and finished.");

    return true;
}

void sendMessage(int fSocket, char* message)
{
    int messageLength = strlen(message);
    int bytesSent = send(fSocket, message, messageLength, 0);
    CHECK_RESULT_AND_EXIT(bytesSent);

    if (bytesSent != messageLength)
    {
        printf("Sent %d out of %d", bytesSent, messageLength);
        return;
    }
}

char* receiveMessage(int fSocket)
{
    char *buffer = malloc(KILOBYTE * sizeof(char));
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, KILOBYTE, 0);
    CHECK_RESULT_AND_EXIT(bytesRead);

    buffer[bytesRead] = 0;
    return buffer;
}