#include "chat.h"

// int                 server_fd[5];
// struct sockaddr_in  server_addr[5];

void	*app_socket()
{
	int					server_fd, client_fd, client_len, read_size;
	char				*buffer;
	struct sockaddr_in	server_addr, client_addr;

	server_fd = create_socket();

	set_sock(&server_addr, htonl(INADDR_ANY), (u_short)atoi(argv[1]));

	if (bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("binding socket", strerror(errno));

	if (listen(server_fd, 5) == -1)
		error_handling("listen socket", strerror(errno));

	client_len = 0;
	client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_len);
	if (client_fd == -1)	// error when accept client's connect request
		error_handling("client's requst", strerror(errno));
	
	print_connection(client_addr);

	buffer = malloc_buffer();
	read_size = recv_from_fd(buffer, client_fd, server_fd);
	print_connect(buffer);

	while (1)
	{
		// Initialize buffer as 0(NULL) for read from client
		memset(buffer, 0, BUFFER_SIZE);

		// recv data from client
		read_size = read(client_fd, buffer, BUFFER_SIZE);

		// detect error when recv data from client
		if (read_size <= 0) // error occurs at recv data
		{
			close(server_fd);
			free(buffer);
			error_handling("recv data", strerror(errno));
		}

		// print recved data
		putstr("Client : ");
		putstr(buffer);

		// recv "QUIT" msg
		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 6)
		{
			// close connection & print "Disconnected"
			close(server_fd);
			free(buffer);
			putstr("Disconnected\n");
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
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