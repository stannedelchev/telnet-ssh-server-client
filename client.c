#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

void sendMessage(int fSocket, char* message);
char* receiveMessage(int fSocket);

int createAndConnectSocket(app_options_t options);
void closeSocket(int fSocket);
void checkResultAndExit(int result);

bool run_client(app_options_t options)
{
    int fSocket = createAndConnectSocket(options);
    printf("Type your commands. Empty line exits.\n");
    
    const int MESSAGE_LENGTH = 256;
    char message[MESSAGE_LENGTH];
    while (fgets(message, MESSAGE_LENGTH, stdin))
    {
        if (strcmp(message, "\n") == 0 || strcmp(message, "\r\n") == 0)
        {
            break;
        }
        
        sendMessage(fSocket, message);
        char* serverMessage = receiveMessage(fSocket);
        printf("%s", serverMessage);
        free(serverMessage);
    }

    closeSocket(fSocket);
    printf("Closed all and finished.");

    return true;
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

char* receiveMessage(int fSocket)
{
    const int KBYTE = 1024 * 1024;
    char *buffer = malloc(KBYTE * sizeof(char));
    int bytesRead = -1;
    bytesRead = recv(fSocket, buffer, KBYTE, 0);
    checkResultAndExit(bytesRead);

    buffer[bytesRead] = 0;
    return buffer;
}

int createAndConnectSocket(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);

    checkResultAndExit(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(options.host);
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct
    
    int connectOk = connect(fSocket, (struct sockaddr*) &serverAddr, sizeof (struct sockaddr_in));
    checkResultAndExit(connectOk);
    return fSocket;
}

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