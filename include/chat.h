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

# define BUFFER_SIZE 131072

void	print_connection(struct sockaddr_in info);
void	error_handling(char *type, char *errmsg);
int		create_socket(void);
void	putstr(char *s);
void	set_sock(struct sockaddr_in *socket, in_addr_t addr, u_short port);
void	putport(u_short port);
char	*malloc_buffer(void);

#endif