#include "chat.h"

t_socket 			server;
int					client_cnt = 0;
int					client[MAX_CLIENT];
char				*username[MAX_CLIENT];
struct sockaddr_in	client_addr;
int					client_len;
pthread_mutex_t		 mutex;

void	send_msg(char *str, int except)
{
	int i;

    pthread_mutex_lock(&mutex);
    for (i = 0; i < client_cnt; i++)
	{
		if (client[i] != except)
        	write(client[i], str, strlen(str));
	}
    pthread_mutex_unlock(&mutex);
}

void	*recvRoutine(void *arg)
{
	int 	client_fd = *((int *)arg);
	char	*recv_buffer;
	int		recv_size;

	recv_buffer = malloc_buffer();
	memset(recv_buffer, 0, BUFFER_SIZE + 1);
	recv_size = read(client_fd, recv_buffer, BUFFER_SIZE);
	send_msg(recv_buffer, client_fd);
	print_connect(recv_buffer, 1);

	while(1)
	{
		memset(recv_buffer, 0, BUFFER_SIZE + 1);

		recv_size = read(client_fd, recv_buffer, BUFFER_SIZE);	
		// detect error when recv data from client
		if (recv_size <= 0) // error occurs at recv data
		{
			// pthread_cancel(send_threadID);
			close(client_fd);
			free(recv_buffer);
			error_handling("recv data", strerror(errno));
			return (NULL);
		}

		// print recved data
		putstr(recv_buffer, 1);	
		send_msg(recv_buffer, client_fd);
		send_msg("is connected", client_fd);
	}
	return (NULL);
}

int main(int argc, char *argv[])
{
	pthread_t	recv_threadID;
	if (argc != 2)
	{
		putstr("usage : ./server <port>\n", 1);
		exit(FAIL);
	}

	server.socket_fd = create_socket();

	set_sock(&(server.socket_addr), htonl(INADDR_ANY), (u_short)atoi(argv[1]));

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
	while (1)
	{
		client[client_cnt] = accept(server.socket_fd, (struct sockaddr *)&(client_addr), (socklen_t *)&(client_len));
		if (client[client_cnt] == -1)
		{
			error_handling("client's request", strerror(errno));
			exit(FAIL);
		}
		print_connection(client_addr);

		if (pthread_create(&recv_threadID, NULL, recvRoutine, (void *)&(client[client_cnt])) != 0)
		{
			error_handling("create thread", strerror(errno));
			exit(FAIL);
		}
		pthread_detach(recv_threadID);
		client_cnt++;
	}
}