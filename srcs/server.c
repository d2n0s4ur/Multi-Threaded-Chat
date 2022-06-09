#include "chat.h"

t_socket 			server;
int					client_cnt = 0;
int					client[MAX_CLIENT];
struct sockaddr_in	client_addr;
int					client_len;
pthread_mutex_t		mutex;

/*******************************************************************
* FUNCTION : 	void	remove_client(int index)
* PARAMETERS:	index(int)
* RETURN :		None(void)
* DESCRIPTION :	When disconnect client, remove that fd from client arr
********************************************************************/
void	remove_client(int index)
{
	// Critical section
	pthread_mutex_lock(&mutex);
	while (index < client_cnt - 1)
	{
		client[index] = client[index + 1];
		index++;
	}
	client_cnt--;
	pthread_mutex_unlock(&mutex);
}

/*******************************************************************
* FUNCTION : 	void	send_msg(char *str, int except)
* PARAMETERS:	str(char *), except(int)
* RETURN :		None(void)
* DESCRIPTION : broadcast str to stdoutput & clients. except fd(except)
********************************************************************/
void	send_msg(char *str, int except)
{
	int i;

	i = 0;
	write(1, str, strlen(str)); // write to std output

	// Critical section
    pthread_mutex_lock(&mutex);
    while (i < client_cnt)
	{
		if (client[i] != except)
        	write(client[i], str, strlen(str)); // broadcast msg except fd(except)
		i++;
	}
    pthread_mutex_unlock(&mutex);
}

/*******************************************************************
* FUNCTION : 	void	*recvRoutine(void *arg)
* PARAMETERS:	arg(void *) -> fd
* RETURN :		void * 
* DESCRIPTION : Thread for recv clients msg & broadcast it.
********************************************************************/
void	*recvRoutine(void *arg)
{
	int 	client_fd = *((int *)arg);
	char	*recv_buffer;
	int		recv_size;
	char	*username;

	recv_buffer = malloc_buffer(); // malloc buffer
	memset(recv_buffer, 0, BUFFER_SIZE + 1); // initialization buffer

	recv_size = read(client_fd, recv_buffer, BUFFER_SIZE); // recv username from client

	// broadcast client connect info
	send_msg(recv_buffer, -1);
	send_msg(" is connected\n", -1);

	//calc client index
	int i = 0;
	while (client[i] != client_fd)
		i++;
	// malloc username (duplicate)
	username = strdup(recv_buffer);

	// recv msg until "QUIT" recved
	while(1)
	{
		memset(recv_buffer, 0, BUFFER_SIZE + 1); // initialization buffer

		recv_size = read(client_fd, recv_buffer, BUFFER_SIZE); // recv msg from client

		if (recv_size <= 0 || (strncmp(recv_buffer, "QUIT\n", 5) == 0 && recv_size == 6)) // when recv "QUIT" or detect error(ex. Ctrl-C) -> disconnect
		{
			// broadcast client disconnect info except client
			send_msg(username, client_fd);
			send_msg(" is disconnected\n", client_fd);

			remove_client(i); // remove client fd from client fd arr
			free(recv_buffer); // free buffer
			free(username); // free username
			close(client_fd); // close client socket fd
			return (NULL);
		}

		// broadcast recved msg except client
		send_msg(username, client_fd);
		send_msg(": ", client_fd);
		send_msg(recv_buffer, client_fd);
	}
	return (NULL);
}

int main(int argc, char *argv[])
{
	pthread_t	recv_threadID;
	int			fd;

	// Check parameter's count. If parameter's num is not 1, exit program.
	if (argc != 2)
	{
		putstr("usage : ./server <port>\n", 1);
		exit(FAIL);
	}

	// Create Socket
	server.socket_fd = create_socket();

	// socket addr setting
	set_sock(&(server.socket_addr), htonl(INADDR_ANY), (u_short)atoi(argv[1]));

	// binding socket
	if (bind(server.socket_fd, (const struct sockaddr*)&(server.socket_addr), sizeof(server.socket_addr)) == -1)
	{
		error_handling("binding socket", strerror(errno));
		exit(FAIL);
	}

	// Make socket as usable. set pending connection queue length
	if (listen(server.socket_fd, 5) == -1)
	{
		error_handling("listen socket", strerror(errno));
		exit(FAIL);
	}

	// mutext init -> for critical section
	pthread_mutex_init(&mutex,NULL);
	while (1)
	{
		// accept client's connect requset
		fd = accept(server.socket_fd, (struct sockaddr *)(&client_addr), (socklen_t *)(&client_len));
		if (client_cnt >= MAX_CLIENT) // when client count over MAX_CLIENT(5)
		{
			close(fd); // close
			continue;
		}
		if (fd == -1) // wrong accept
		{
			error_handling("client's request", strerror(errno));
			break;
		}

		// Critical section
		pthread_mutex_lock(&mutex);
		client[client_cnt++] = fd; // add client fd to client fd arr
		pthread_mutex_unlock(&mutex);

		print_connection(client_addr); // print client's IP & Port
		
		// recv Thread Create
		if (pthread_create(&recv_threadID, NULL, recvRoutine, (void *)&(fd)) != 0)
		{
			error_handling("create thread", strerror(errno));
			break;
		}

		//thread detach -> when thread end, return resources
		pthread_detach(recv_threadID);
	}
	// mutext distroy
	pthread_mutex_destroy(&mutex);
	exit(PASS);
}