#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ftpserver.h"
int main(int argc, char**argv)
{
    printf("start \n");
    for (int i=0; i<argc; i++)
    {
      printf("%d. %s\n", i, argv[i]);
    }


    struct FtpServer *s = (struct FtpServer*) malloc(sizeof(struct FtpServer));
    if(argc >= 1)
    {
    	show_log(argv[1]);
      s->_port = atoi(argv[1]);
    }
    else
    {
    	 s->_port = 21;
    }
    if(argc < 2)
    //strcpy(s->_relative_path, argv[2]);
    {
    	strcpy(s->_relative_path, "/tmp");
    }
    else
    {
    	 show_log(argv[2]);
    	 strcpy(s->_relative_path, argv[2]);
    }
    printf("\ns->_port: %d, s->_relative_path: %s\n", s->_port, s->_relative_path); 
    init_ftp_server(s);
    start_ftp_server(s);
   // close_ftp_server(s);
    return 0;
}
