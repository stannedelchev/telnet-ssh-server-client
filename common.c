#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.h"

void closeSocket(int fSocket)
{
    shutdown(fSocket, 2);
    close(fSocket);
}

void checkResultAndExit(int result)
{
    if(result == -1)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

char* receiveMessage(int fSocket)
{
    char *buffer = malloc(KBYTE * sizeof(char));
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, KBYTE, 0);
    checkResultAndExit(bytesRead);

    buffer[bytesRead] = 0;
    return buffer;
}

void sendMessage(int fSocket, char* message)
{
    int messageLength = strlen(message);
    int bytesSent = send(fSocket, message, messageLength, 0);
    checkResultAndExit(bytesSent);

    if (bytesSent != messageLength)
    {
        printf("Sent %d out of %d", bytesSent, messageLength);
        return;
    }
}