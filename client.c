#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

void sendMessage(int fSocket, char* message);
char* receiveMessage(int fSocket);

int createAndConnectSocket(app_options_t options);

void run_client(app_options_t options)
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