#include "chat.h"

int	main(int argc, char *argv[])
{
	int					server_fd, client_fd, client_len, read_size;
	char				*buffer;
	struct sockaddr_in	server_addr, client_addr;

	// Check parameter's count. If parameter's num is not 1, exit program.
	if (argc != 2)
	{
		putstr("usage : ./server <port>\n");
		exit(EXIT_FAILURE);
	}

	// Create Socket
	server_fd = create_socket();

	// socket addr setting
	set_sock(&server_addr, htonl(INADDR_ANY), (u_short)atoi(argv[1]));
	
	// Binding to Socket
	if (bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("binding socket", strerror(errno));

	// Make socket as usable. set pending connection queue length
	if (listen(server_fd, 5) == -1)
		error_handling("listen socket", strerror(errno));

	// accept client connection request
	client_len = 0;
	client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_len);
	if (client_fd == -1)	// error when accept client's connect request
		error_handling("client's requst", strerror(errno));
	
	// print connection info(client's IP & PORT)
	print_connection(client_addr);

	// malloc buffer
	buffer = malloc_buffer();

	// Loop until recv QUIT msg or send QUIT msg
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

		// Initialize buffe as 0(NULL) for std input
		memset(buffer, 0, BUFFER_SIZE);
		
		// get data from std input
		putstr("Server : ");
		read_size = read(0, buffer, BUFFER_SIZE);
		if (read_size <= 0) // error occurs at std input
		{
			close(server_fd);
			free(buffer);
			error_handling("std input", strerror(errno));
		}
		if (write(client_fd, buffer, read_size + 1) <= 0) // error occurs when sending data
		{
			close(server_fd);
			free(buffer);
			error_handling("sending data", strerror(errno));
		}
		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 5) // when send "QUIT" -> disconnect
		{
			close(server_fd);
			free(buffer);
			putstr("Disconnected\n");
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_SUCCESS);
}
