#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "types.h"

bool parse_arguments(int argc, char** argv, app_options_t* options)
{
    int minAllowedArgs = 3;
    for (int i = 0; i < argc; i++)
    {
        printf("%d - %s\n", i, argv[i]);
    }

    if (argc < minAllowedArgs ||
            (strcmp(argv[1], "server") != 0 &&
            strcmp(argv[1], "client") != 0))
    {
        return false;
    }

    if (strcmp(argv[1], "server") == 0)
    {
        //        options->type = SERVER;
    } else if (strcmp(argv[1], "client") == 0)
    {
        //        options->type = CLIENT;
    }

    return true;
}

void print_usage()
{
    printf("Usage: <type> <options>\n");
    printf("Type is either \"server\" or \"client\"\n");
    printf("Options can be either: \n");
}

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
    serverAddr.sin_port = htons(80); // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.107");
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

void run_server(app_options_t options)
{

}

int main(int argc, char** argv)
{
    app_options_t options;
    if (!parse_arguments(argc, argv, &options))
    {
        print_usage();
        return EXIT_SUCCESS;
    }

    switch (options.type)
    {
        case CLIENT_APP:
            run_client(options);
            break;
        case SERVER_APP:
            run_server(options);
            break;
        default:
            printf("Unknown application type %d", options.type);
            return EXIT_FAILURE;
    }

    return (EXIT_SUCCESS);
}

