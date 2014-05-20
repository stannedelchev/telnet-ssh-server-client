#ifndef TYPES_H
#define	TYPES_H


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


#endif	/* TYPES_H */

