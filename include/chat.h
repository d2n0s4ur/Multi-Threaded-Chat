#ifndef CHAT_H
# define CHAT_H

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>

# define BUFFER_SIZE 1024
# define FAIL -1
# define PASS 0
# define MAX_CLIENT 5

typedef struct	s_socket {
	int					socket_fd;
	struct sockaddr_in	socket_addr;
}	t_socket;

void	print_connection(struct sockaddr_in info);
void	error_handling(char *type, char *errmsg);
int		create_socket(void);
void	putstr(char *s, int fd);
void	set_sock(struct sockaddr_in *socket, in_addr_t addr, u_short port);
void	putport(u_short port);
char	*malloc_buffer(void);
int		recv_from_fd(char *buffer, int client_fd, int server_fd);
void	print_connect(char *username, int fd);
void	print_disconnect(char *username, int fd);

#endif