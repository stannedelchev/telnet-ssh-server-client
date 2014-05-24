#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "client.h"

bool g_shouldExit = false;

int create_socket_and_bind(app_options_t options);
void execute_client_command(char* clientMessage, int fSocket);

void SIGINT_handler()
{
    g_shouldExit = true;
}

void run_server(app_options_t options)
{
    int fSocket = create_socket_and_bind(options);
    int isListenOk = listen(fSocket, 10);
    check_result_and_exit(isListenOk);

    signal(SIGINT, SIGINT_handler);

    struct sockaddr_in theirAddress;
    int addressSize = sizeof (struct sockaddr_in);
    int fTheirSocket = accept(fSocket, (struct sockaddr *) &theirAddress, &addressSize);

    while (!g_shouldExit)
    {
        char* clientMessage = receive_message(fTheirSocket);
        if (clientMessage == NULL ||
                strlen(clientMessage) == 0 ||
                strcmp(clientMessage, "\n") == 0 ||
                strcmp(clientMessage, "\r\n") == 0)
        {
            g_shouldExit = true;
        } else
        {
            send_message(fTheirSocket, BEG_SYMBOL);
            execute_client_command(clientMessage, fTheirSocket);
            send_message(fTheirSocket, END_SYMBOL);
        }
    }

    close_socket(fTheirSocket);
    close_socket(fSocket);
}

int create_socket_and_bind(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);
    check_result_and_exit(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct

    // reuse socket port
    int VALUE_REUSE_SOCKET = 1;
    int isSocketReuseOk = setsockopt(fSocket, SOL_SOCKET, SO_REUSEADDR, &VALUE_REUSE_SOCKET, sizeof (int));
    check_result_and_exit(isSocketReuseOk);

    int isBindOk = bind(fSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr_in));
    check_result_and_exit(isBindOk);

    return fSocket;
}

void execute_client_command(char* clientMessage, int fSocket)
{
    char* trimmedCommand = strtok(clientMessage, "\r\n");
    char* command = calloc(KBYTE, sizeof(char));
    sprintf(command, "%s 2>&1", trimmedCommand);
    FILE *fProcess = popen(command, "r");

    if (fProcess == NULL)
    {
        return;
    }

    printf("Trying to execute \"%s\"\n", clientMessage);
    char* buffer = calloc(KBYTE, sizeof (char));
    while (fgets(buffer, KBYTE, fProcess) != NULL)
    {
        send_message(fSocket, buffer);
    }

    free(buffer);
    free(command);

    pclose(fProcess);
}