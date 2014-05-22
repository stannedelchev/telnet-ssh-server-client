#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "client.h"
#include "server.h"

bool parse_arguments(int argc, char** argv, app_options_t* options)
{
    const int minAllowedArgs = 3;
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

    memset(options, 0, sizeof(app_options_t));
    
    if (strcmp(argv[1], "server") == 0)
    {
        options->port = atoi(argv[2]);
        options->type = SERVER_APP;
    } else if (strcmp(argv[1], "client") == 0)
    {
        options->host = argv[2];
        options->port = atoi(argv[3]);
        options->type = CLIENT_APP;
    }
    
    return true;
}

void print_usage()
{
    printf("Usage: <type> <options>\n");
    printf("Type is either \"server\" or \"client\"\n");
    printf("Options can be either: \n");
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

    printf("Closed all sockets and finished.\n");
    return (EXIT_SUCCESS);
}

