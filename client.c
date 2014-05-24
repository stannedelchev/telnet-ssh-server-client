#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

int create_socket_and_connect(app_options_t options);
void send_message(int fSocket, char* message);
char* receive_message(int fSocket);

void run_client(app_options_t options)
{
    int fSocket = create_socket_and_connect(options);
    printf("Type your commands. Empty line exits.\n");
    
    const int MESSAGE_LENGTH = 256;
    char message[MESSAGE_LENGTH];
    while (fgets(message, MESSAGE_LENGTH, stdin))
    {
        if (strcmp(message, "\n") == 0 || strcmp(message, "\r\n") == 0)
        {
            break;
        }
        
        send_message(fSocket, message);
        char* serverMessage = receive_message(fSocket);
        while(strcmp(serverMessage, END_SYMBOL) != 0)
        {
            printf("%s", serverMessage);
            serverMessage = receive_message(fSocket);
        }
        printf("%s\n", serverMessage);

        free(serverMessage);
    }

    close_socket(fSocket);
}

int create_socket_and_connect(app_options_t options)
{
    int fSocket = socket(AF_INET, SOCK_STREAM, 0);

    check_result_and_exit(fSocket);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET; // host byte order
    serverAddr.sin_port = htons(options.port); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(options.host);
    memset(&(serverAddr.sin_zero), '\0', 8); // zero the rest of the struct
    
    int connectOk = connect(fSocket, (struct sockaddr*) &serverAddr, sizeof (struct sockaddr_in));
    check_result_and_exit(connectOk);
    return fSocket;
}