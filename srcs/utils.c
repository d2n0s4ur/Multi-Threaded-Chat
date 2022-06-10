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
	putstr("[-]Error encount when ", 2);
	putstr(type, 2);
	putstr(" : ", 2);
	putstr(errmsg, 2);
	putstr("\n", 2);
}

/*******************************************************************
* FUNCTION : 	void	putstr(char *s, int fd)
* PARAMETERS:	s(char *), fd(int)
* RETURN :		None(void)
* DESCRIPTION :	write string at fd buffer (file descriptor)
********************************************************************/
void	putstr(char *s, int fd)
{
	write(fd, s, strlen(s));
}

/*******************************************************************
* FUNCTION : 	void	print_connection(struct sockaddr_in info)
* PARAMETERS:	info(sockaddr_in)
* RETURN :		None(void)
* DESCRIPTION :	write Connection msg to stdout
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
* DESCRIPTION :	write port number on std output (fd 1)
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
