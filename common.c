#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.h"

void close_socket(int fSocket)
{
    shutdown(fSocket, 2);
    close(fSocket);
}

void check_result_and_exit(int result)
{
    if(result == -1)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

char* receive_message(int fSocket)
{
    char *buffer = calloc(KBYTE, sizeof(char));
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, KBYTE, 0);
    check_result_and_exit(bytesRead);

    buffer[bytesRead] = 0;
    return buffer;
}

void send_message(int fSocket, char* message)
{
    int messageLength = strlen(message);
    int bytesSent = send(fSocket, message, messageLength, 0);
    check_result_and_exit(bytesSent);

    if (bytesSent != messageLength)
    {
        printf("Sent %d out of %d", bytesSent, messageLength);
        return;
    }
}