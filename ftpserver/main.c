#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "ftpserver.h"
#include "options.hpp"

void usage(char * s)
{

	fprintf( stderr, "\n");
  	fprintf( stderr, "%s : %s-%s \n", s, __DATE__, __TIME__);
	fprintf( stderr, "%s -c <config file>\n", s);
	fprintf( stderr, "example: %s -c conf/aiintruder.conf\n", s);
	fprintf( stderr, "\n");
}


int main(int argc, char**argv)
{
    signal(SIGPIPE, SIG_IGN);

    std::string config_path;

    int c;
    while ( ((c = getopt( argc, argv, "c:?" ) ) ) != -1 )
    {
        switch (c)
        {
            case 'c':
                config_path = optarg;
                break;
            case '?':
            default:
                usage(argv[0]);
                return -1;
        }
    }

    if(config_path.empty())
    {
        usage(argv[0]);
        return -1;
    }


    keymolen::Options *options = keymolen::Options::Instance();
    if(options->Read(config_path.c_str()) != 0)
    {
        printf("could not open config file: %s\n", config_path.c_str());
        usage(argv[0]);
        return -1;
    }

    struct FtpServer *s = (struct FtpServer*) malloc(sizeof(struct FtpServer));
    s->_port = options->ftpserver.port;
    strncpy(s->_relative_path, options->ftpserver.root.c_str(), 100);
    s->_options = options;
    
    printf("\ns->_port: %d, s->_relative_path: %s\n", s->_port, s->_relative_path); 
    
    init_ftp_server(s);
    start_ftp_server(s);

    printf("end...\n");
   // close_ftp_server(s);
    return 0;
}
