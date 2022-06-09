#include "chat.h"

/*******************************************************************
* FUNCTION : 	int	create_socket(void)
* PARAMETERS:	None(void)
* RETURN :		socket_fd(int)
* DESCRIPTION :	Create TCP/IP socket and handling error
********************************************************************/
int	create_socket(void)
{
	int	ret;

	ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret == -1) // detecting error when create socket
		error_handling("creating socket", strerror(errno));
	return (ret);
}

/*******************************************************************
* FUNCTION : 	void	error_handling(char *type, char *errmsg)
* PARAMETERS:	type(char *), errmsg(char *)
* RETURN :		None(void)
* DESCRIPTION :	write error msg
********************************************************************/
void	error_handling(char *type, char *errmsg)
{
	putstr("[-]Error encount when ", 1);
	putstr(type, 1);
	putstr(" : ", 1);
	putstr(errmsg, 1);
	putstr("\n", 1);
}

/*******************************************************************
* FUNCTION : 	void	putstr(char *s)
* PARAMETERS:	s(char *)
* RETURN :		None(void)
* DESCRIPTION :	write string at stdin buffer
********************************************************************/
void	putstr(char *s, int fd)
{
	write(fd, s, strlen(s));
}

/*******************************************************************
* FUNCTION : 	void	print_connection(struct sockaddr_in info)
* PARAMETERS:	info(sockaddr_in)
* RETURN :		None(void)
* DESCRIPTION :	write Connection msg
********************************************************************/
void	print_connection(struct sockaddr_in info)
{
	putstr("Connection from ", 1);
	putstr(inet_ntoa(info.sin_addr), 1);
	putstr(":", 1);
	putport(info.sin_port);
	putstr("\n", 1);
}

/*******************************************************************
* FUNCTION : 	void	set_sock(struct sockaddr_in *socket, in_addr_t addr, u_short port)
* PARAMETERS:	socket(*sockaddr_in), addr(address), port(port num)
* RETURN :		None(void)
* DESCRIPTION :	set IP socket addr struct's ip & port
********************************************************************/
void	set_sock(struct sockaddr_in *socket, in_addr_t addr, u_short port)
{
	memset(socket, 0, sizeof(*socket));
	socket->sin_family = AF_INET;
	socket->sin_addr.s_addr = addr;
	socket->sin_port = port;
}

/*******************************************************************
* FUNCTION : 	void	putport(u_short port)
* PARAMETERS:	port(unsigned short)
* RETURN :		None(void)
* DESCRIPTION :	write port number on std output
********************************************************************/
void	putport(u_short port)
{
	char	ret[6];
	int		i;

	i = 0;
	if (port == 0)
	{
		write(1, "0", 1);
		return ;
	}
	while (port)
	{
		ret[i] = port % 10 + '0';
		port /= 10;
		i++;
	}
	while(--i >= 0)
		write(1, &(ret[i]), 1);
}

/*******************************************************************
* FUNCTION : 	char	*malloc_buffer(void)
* PARAMETERS:	None(void)
* RETURN :		malloced string(char *)
* DESCRIPTION :	malloc buffer
********************************************************************/
char	*malloc_buffer(void)
{
	char	*ret;

	ret = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!ret)
	{
		error_handling("malloc buffer", strerror(errno));
		exit(FAIL);
	}
	return (ret);
}

int	recv_from_fd(char *buffer, int client_fd, int server_fd)
{
	int ret;

	// Initialize buffer as 0(NULL) for read from client
	memset(buffer, 0, BUFFER_SIZE);

	// recv data from client
	ret = read(client_fd, buffer, BUFFER_SIZE);

	// detect error when recv data from client
	if (ret <= 0) // error occurs at recv data
	{
		close(server_fd);
		close(client_fd);
		free(buffer);
		error_handling("recv data", strerror(errno));
	}
	return (ret);
}

void	print_connect(char *username, int fd)
{
	putstr(username, fd);
	putstr(" is connected\n", fd);
}

void	print_disconnect(char *username, int fd)
{
	putstr(username, fd);
	putstr(" is disconnected\n", fd);
}
