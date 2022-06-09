#include "chat.h"

int				recv_size, send_size;
t_socket		client;
char			*send_buffer, *recv_buffer;
pthread_t		send_threadID, recv_threadID;
void			*send_ret, *recv_ret;

/*******************************************************************
* FUNCTION : 	void	*sendRoutine(void *arg)
* PARAMETERS:	arg(void *) -> use as username
* RETURN :		void *
* DESCRIPTION :	Thread for send msg to server
********************************************************************/
void	*sendRoutine(void *arg)
{
	// loop while send QUIT msg
	while (1)
	{
		// initailize buffer as 0(NULL)
		memset(send_buffer, 0, BUFFER_SIZE + 1);

		// get data from std input
		send_size = read(0, send_buffer, BUFFER_SIZE);

		if (send_size <= 0) // error occurs at std input -> thread end
		{
			pthread_cancel(recv_threadID); // cancel recv thread before close socket fd
			close(client.socket_fd); // close socket fd
			free(send_buffer); //free buffer
			error_handling("std input", strerror(errno)); // error msg print
			return (NULL);
		}
		if (write(client.socket_fd, send_buffer, send_size + 1) <= 0) // error occurs when sending data
		{
			pthread_cancel(recv_threadID); // cancel recv thread before close socket fd
			close(client.socket_fd); // close socket fd
			free(send_buffer); //free buffer
			error_handling("sending data", strerror(errno)); // error msg print
			return (NULL);
		}
		if (strncmp(send_buffer, "QUIT\n", 5) == 0 && send_size == 5) // when send "QUIT" -> disconnect
		{
			putstr(arg, 1);
			putstr(" is disconnected\n", 1); // print disconnect msg to std output
			pthread_cancel(recv_threadID); // cancel recv thread before close socket fd
			close(client.socket_fd); // close socket fd
			free(send_buffer); //free buffer
			return (NULL);
		}
	}
}

/*******************************************************************
* FUNCTION : 	void	*recvRoutine(void *arg)
* PARAMETERS:	arg(void *) -> not use
* RETURN :		void *
* DESCRIPTION :	Thread for recv msg from server
********************************************************************/
void	*recvRoutine(void *arg)
{
	// prevent compile error(because my Makefile use -Wall -Wextra -Werror flag)
	(void)arg;

	while (1)
	{
		// initialize buffer as 0(NULL)
		memset(recv_buffer, 0, BUFFER_SIZE + 1);

		// recv data from server
		recv_size = read(client.socket_fd, recv_buffer, BUFFER_SIZE);

		// detect error when recv data from server
		if (recv_size <= 0) // error occurs at recv data
		{
			pthread_cancel(send_threadID); // cancel send thread before close socket fd
			close(client.socket_fd); // close socket fd
			free(recv_buffer); //free buffer
			error_handling("recv data", strerror(errno)); // error msg print
			return (NULL);
		}

		// print recved data to std output
		putstr(recv_buffer, 1);		
	}
}

int	main(int argc, char *argv[])
{
	// Check parameter's count. If parameter's num is not 3, exit program.
	if (argc != 4)
	{
		putstr("usage : ./client <ip> <port> <username>\n", 1);
		exit(FAIL);
	}

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

	// error occurs when sending username
	if (write(client.socket_fd, argv[3], strlen(argv[3])) <= 0) 
	{
		close(client.socket_fd);
		error_handling("sending username", strerror(errno));
		exit(FAIL);
	}

	// malloc buffers
	send_buffer = malloc_buffer();
	recv_buffer = malloc_buffer();

	// send Thread Create
	if (pthread_create(&send_threadID, NULL, sendRoutine, argv[3]) != 0)
	{
		error_handling("create thread", strerror(errno));
		exit(FAIL);
	}
	// recv Thread Create
	if (pthread_create(&recv_threadID, NULL, recvRoutine, NULL) != 0)
	{
		error_handling("create thread", strerror(errno));
		exit(FAIL);
	}

	// thread join -> when thread ends, send_ret & recv_ret set.
	pthread_join(send_threadID, &send_ret);
	pthread_join(recv_threadID, &recv_ret);

	// when thread cancled (it means somthing wrong while connection)
	if (send_ret == PTHREAD_CANCELED || recv_ret == PTHREAD_CANCELED)
		exit(FAIL);

	return (0);
}
