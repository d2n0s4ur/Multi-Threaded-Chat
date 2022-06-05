CC=gcc
CFLAGS=-Wall -Werror -Wextra -I ./include
RM=rm -f

UTIL_SRCS=srcs/utils.c
SERVER_SRCS=srcs/server.c $(UTIL_SRCS)
CLIENT_SRCS=srcs/client.c $(UTIL_SRCS)
GNL_SRCS=get_next_line/get_next_line.c get_next_line/get_next_line_utils.c

SERVER_OBJS=$(SERVER_SRCS:.c=.o)
CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)
GNL_OBJS=$(GNL_SRCS:.c=.o)

SERVER=server
CLIENT=client
GNL_NAME=libgnl.a
LIB=ar crs

all: gnl $(SERVER) $(CLIENT)

gnl: $(GNL_OBJS)
	$(LIB) $(GNL_NAME) $^

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -L . -l gnl -o $(SERVER)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -L . -l gnl -o $(CLIENT)

clean:
	$(RM) $(OBJS) $(SERVER_OBJS) $(CLIENT_OBJS) $(GNL_OBJS)

fclean: clean
	$(RM) $(SERVER) $(CLIENT) $(GNL_NAME)

re: fclean all

.PHONY: all clean fclean re