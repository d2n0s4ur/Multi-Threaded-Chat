#include "chat.h"

int				recv_size, send_size, send_val;
t_socket		client;
char			*send_buffer, *recv_buffer;
pthread_t		send_threadID, recv_threadID;
void			*send_ret, *recv_ret;

void	socket_connction(char *argv[])
{
	// Create Socket
	client.socket_fd = create_socket();

	// socket addr setting
	set_sock(&(client.socket_addr), inet_addr(argv[1]), (u_short)atol(argv[2]));

	// Connect socket
	if (connect(client.socket_fd, (struct sockaddr *)(&(client.socket_addr)), sizeof(client.socket_addr)) < 0)
	{
		error_handling("connect to server", strerror(errno));
		exit(FAIL);
	}

	// print connected msg when connection success
	print_connect(argv[3], 1);

	if (write(client.socket_fd, argv[3], strlen(argv[3])) <= 0) // error occurs when sending username
	{
		close(client.socket_fd);
		error_handling("sending username", strerror(errno));
		exit(FAIL);
	}
}

void	*sendRoutine(void *arg)
{
	// loop while recv QUIT msg or send QUIT msg
	while (1)
	{
		// initailize buffer as 0(NULL)
		memset(send_buffer, 0, BUFFER_SIZE + 1);

		// get data from std input
		send_size = read(0, send_buffer, BUFFER_SIZE);

		if (send_size <= 0) // error occurs at std input
		{
			pthread_cancel(recv_threadID);
			close(client.socket_fd);
			free(send_buffer);
			error_handling("std input", strerror(errno));
			return (NULL);
		}
		if (write(client.socket_fd, send_buffer, send_size + 1) <= 0) // error occurs when sending data
		{
			pthread_cancel(recv_threadID);
			close(client.socket_fd);
			free(send_buffer);
			error_handling("sending data", strerror(errno));
			return (NULL);
		}
		if (strncmp(send_buffer, "QUIT\n", 5) == 0 && send_size == 5) // when send "QUIT" -> disconnect
		{
			print_disconnect(arg, 1);
			pthread_cancel(recv_threadID);
			close(client.socket_fd);
			free(send_buffer);
			return (NULL);
		}
	}
}

void	*recvRoutine(void *arg)
{
	(void)arg;

	while (1)
	{
		// initialize buffer as 0(NULL)
		memset(recv_buffer, 0, BUFFER_SIZE + 1);

		// recv data from server
		recv_size = read(client.socket_fd, recv_buffer, BUFFER_SIZE);

		// detect error when recv data from client
		if (recv_size <= 0) // error occurs at recv data
		{
			pthread_cancel(send_threadID);
			close(client.socket_fd);
			free(recv_buffer);
			error_handling("recv data", strerror(errno));
			return (NULL);
		}

		// print recved data
		putstr(recv_buffer, 1);		
	}
}

int	main(int argc, char *argv[])
{
	// Check parameter's count. If parameter's num is not 2, exit program.
	if (argc != 4)
	{
		putstr("usage : ./client <ip> <port> <username>\n", 1);
		exit(FAIL);
	}

	// before recv & send data
	socket_connction(argv);

	send_buffer = malloc_buffer();
	recv_buffer = malloc_buffer();

	if (pthread_create(&send_threadID, NULL, sendRoutine, argv[3]) != 0)
	{
		error_handling("create thread", strerror(errno));
		exit(FAIL);
	}
	if (pthread_create(&recv_threadID, NULL, recvRoutine, NULL) != 0)
	{
		error_handling("create thread", strerror(errno));
		exit(FAIL);
	}

	pthread_join(send_threadID, &send_ret);
	pthread_join(recv_threadID, &recv_ret);

	if (send_ret == PTHREAD_CANCELED || recv_ret == PTHREAD_CANCELED)
		exit(FAIL);

	return (0);
}
