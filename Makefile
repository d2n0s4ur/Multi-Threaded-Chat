CC=gcc
CFLAGS=-Wall -Werror -Wextra -I ./include -pthread
RM=rm -f

UTIL_SRCS=srcs/utils.c
SERVER_SRCS=srcs/server.c $(UTIL_SRCS)
CLIENT_SRCS=srcs/client.c $(UTIL_SRCS)

SERVER_OBJS=$(SERVER_SRCS:.c=.o)
CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)

SERVER=server
CLIENT=client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $(SERVER)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $(CLIENT)

clean:
	$(RM) $(OBJS) $(SERVER_OBJS) $(CLIENT_OBJS) $(GNL_OBJS)

fclean: clean
	$(RM) $(SERVER) $(CLIENT) $(GNL_NAME)

re: fclean all

.PHONY: all clean fclean re