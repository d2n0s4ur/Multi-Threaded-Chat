#include "chat.h"

pthread_mutex_t	mutex;
int				read_size, read_size;
t_socket		client;
char			*buffer;

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
	print_connect(argv[3]);

	if (write(client.socket_fd, argv[3], strlen(argv[3])) <= 0) // error occurs when sending username
	{
		close(client.socket_fd);
		error_handling("sending username", strerror(errno));
		exit(FAIL);
	}
}

void	*sendRoutine(void *arg)
{
	(void)arg;
	pthread_mutex_lock(&mutex);

	// loop while recv QUIT msg or send QUIT msg
	while (1)
	{
		// initailize buffer as 0(NULL)
		memset(buffer, 0, BUFFER_SIZE);

		// get data from std input
		read_size = read(0, buffer, BUFFER_SIZE);

		if (read_size <= 0) // error occurs at std input
		{
			close(client.socket_fd);
			free(buffer);
			error_handling("std input", strerror(errno));
			pthread_mutex_unlock(&mutex);
			exit(FAIL);
		}
		if (write(client.socket_fd, buffer, read_size + 1) <= 0) // error occurs when sending data
		{
			close(client.socket_fd);
			free(buffer);
			error_handling("sending data", strerror(errno));
			pthread_mutex_unlock(&mutex);
			exit(FAIL);
		}
		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 5) // when send "QUIT" -> disconnect
		{
			close(client.socket_fd);
			free(buffer);
			putstr("Disconnected\n");
			pthread_mutex_unlock(&mutex);
			exit(PASS);
		}
	}
}

void	*recvRoutine(void *arg)
{
	(void)arg;
	pthread_mutex_lock(&mutex);
	while (1)
	{
		// initialize buffer as 0(NULL)
		memset(buffer, 0, BUFFER_SIZE);

		// recv data from server
		read_size = read(client.socket_fd, buffer, BUFFER_SIZE);

		// detect error when recv data from client
		if (read_size <= 0) // error occurs at recv data
		{
			close(client.socket_fd);
			free(buffer);
			error_handling("recv data", strerror(errno));
			pthread_mutex_unlock(&mutex);
			exit(FAIL);
		}

		// print recved data
		putstr(buffer);

		if (strncmp(buffer, "QUIT\n", 5) == 0 && read_size == 6) // when recv "QUIT" msg
		{
			//close connection & print "Disconnected"
			close(client.socket_fd);
			free(buffer);
			putstr("Disconnected\n");
			pthread_mutex_unlock(&mutex);
			exit(PASS);
		}
	}
}

int	main(int argc, char *argv[])
{
	pthread_t send_threadID, recv_threadID;
	buffer = malloc_buffer();

	// Check parameter's count. If parameter's num is not 2, exit program.
	if (argc != 4)
	{
		putstr("usage : ./client <ip> <port> <username>\n");
		pthread_exit(FAIL);
	}
	
	// before recv & send data
	socket_connction(argv);

	pthread_mutex_init(&mutex,NULL);

	pthread_create(&send_threadID, NULL, recvRoutine, NULL);
	pthread_create(&recv_threadID, NULL, sendRoutine, NULL);

	pthread_join(send_threadID, NULL);
	pthread_join(recv_threadID, NULL);

	pthread_mutex_destroy(&mutex);

	return (0);
}
