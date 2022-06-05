#include "chat.h"

void    wait_binding

int main(int argc, char *argv[])
{
    int                 server_fd[5];
    struct sockaddr_in  server_addr[5];
    int i;

    // Check parameter's count. If parameter's num is not 1, exit program.
	if (argc != 2)
	{
		putstr("Usage : ./server <port>\n");
		exit(EXIT_FAILURE);
	}
    i = 0;
    while (i < 5)
    {
        server_fd[i] = create_socket();
        set_sock(&(server_addr[i]), htonl(INADDR_ANY), (u_short)atoi(argv[1]));
        i++;
    }

}