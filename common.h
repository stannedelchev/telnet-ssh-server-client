#ifndef TYPES_H
#define	TYPES_H

#define KBYTE (1024 * 1024)

typedef enum
{
    SERVER_APP = 0,
    CLIENT_APP = 1
} application_type_t;

typedef struct
{
    application_type_t type;
    int port;
    char* host;
} app_options_t;

void closeSocket(int fSocket);
void checkResultAndExit(int result);
char* receiveMessage(int fSocket);
void sendMessage(int fSocket, char* message);

#endif
