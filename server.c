#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "client.h"

bool g_shouldExit = false;

int createAndBindSocket(app_options_t options);

void SIGINT_handler()
{
    g_shouldExit = true;
}

void run_server(app_options_t options)
{
    int fSocket = createAndBindSocket(options);
    int isListenOk = listen(fSocket, 10);
    checkResultAndExit(isListenOk);

    signal(SIGINT, SIGINT_handler);

    struct sockaddr_in theirAddress;
    int addressSize = sizeof (struct sockaddr_in);
    int fTheirSocket = accept(fSocket, (struct sockaddr *) &theirAddress, &addressSize);

    while (!g_shouldExit)
    {
        char* clientMessage = receiveMessage(fTheirSocket);
        if (clientMessage == NULL ||
                strlen(clientMessage) == 0 ||
                strcmp(clientMessage, "\n") == 0 ||
                strcmp(clientMessage, "\r\n") == 0)
        {
            g_shouldExit = true;
        } else
        {
            char prefix[] = "You sent: ";
            int prefixLength = strlen(prefix);
            int clientMessageLength = strlen(clientMessage);
            char* message = malloc(sizeof (char) * (prefixLength + clientMessageLength + 2));
            sprintf(message, "%s %s", prefix, clientMessage);
            printf("%s", clientMessage);
            sendMessage(fTheirSocket, message);
            free(message);
        }
    }

    closeSocket(fTheirSocket);
    closeSocket(fSocket);
}

int createAndBindSocket(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);
    checkResultAndExit(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    // reuse socket port
    int REUSE_SOCKET_VALUE = 1;
    int isSocketReuseOk = setsockopt(fSocket, SOL_SOCKET, SO_REUSEADDR, &REUSE_SOCKET_VALUE, sizeof (int));
    checkResultAndExit(isSocketReuseOk);

    int isBindOk = bind(fSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr_in));
    checkResultAndExit(isBindOk);


    return fSocket;
}