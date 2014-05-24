#ifndef TYPES_H
#define	TYPES_H

#define KBYTE (1024 * 1024)
#define BEG_SYMBOL "---START---\n"
#define END_SYMBOL "---END---\n"
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

void close_socket(int fSocket);
void check_result_and_exit(int result);
char* receive_message(int fSocket);
void send_message(int fSocket, char* message);

#endif
