#include "chat.h"

int	main(int argc, char *argv[])
{
	int					client_fd, read_size;
	char				*buffer;
	struct sockaddr_in	server_addr;

	// Check parameter's count. If parameter's num is not 2, exit program.
	if (argc != 3)
	{
		putstr("usage : ./client <ip> <port>\n");
		exit(EXIT_FAILURE);
	}

	// Create Socket
	client_fd = create_socket();

	// socket addr setting
	set_sock(&server_addr, inet_addr(argv[1]), (u_short)atol(argv[2]));

	// Connect socket
	if (connect(client_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr)) < 0)
		error_handling("connect to server", strerror(errno));

	// print connected msg when connection success
	putstr("Connected\n");

	// malloc buffer
	buffer = malloc_buffer();

	// loop while recv QUIT msg or send QUIT msg
	while (1)
	{
		// initailize buffer as 0(NULL)
		memset(buffer, 0, BUFFER_SIZE);

		// get data from std input
		putstr("Client : ");
		read_size = read(0, buffer, BUFFER_SIZE);

		if (read_size <= 0) // error occurs at std input
		{
			close(client_fd);
			free(buffer);
			error_handling("std input", strerror(errno));
		}
		if (write(client_fd, buffer, read_size + 1) <= 0) // error occurs when sending data
		{
			close(client_fd);
			free(buffer);
			error_handling("sending data", strerror(errno));
		}
		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 5) // when send "QUIT" -> disconnect
		{
			close(client_fd);
			free(buffer);
			putstr("Disconnected\n");
			exit(EXIT_SUCCESS);
		}

		// initialize buffer as 0(NULL)
		memset(buffer, 0, BUFFER_SIZE);

		// recv data from server
		read_size = read(client_fd, buffer, BUFFER_SIZE);

		// detect error when recv data from client
		if (read_size <= 0) // error occurs at recv data
		{
			close(client_fd);
			free(buffer);
			error_handling("recv data", strerror(errno));
		}

		// print recved data
		putstr("Server : ");
		putstr(buffer);

		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 6) // when recv "QUIT" msg
		{
			//close connection & print "Disconnected"
			close(client_fd);
			free(buffer);
			putstr("Disconnected\n");
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_SUCCESS);
}
