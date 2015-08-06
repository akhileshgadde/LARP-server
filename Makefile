CC = gcc

LIBS = -lpthread\

FLAGS = -g -O2
CFLAGS = ${FLAGS} -I -Wall

OBJS = htonl3.o label_mapping_table.o larp_reply.o process_pkt.o 

all: server

server: server.o $(OBJS)
	${CC} ${FLAGS} -o server server.o $(OBJS) ${LIBS}

server.o: test_server1.c $(OBJS)
	${CC} ${CFLAGS} -o server.o -c test_server1.c ${LIBS}

htonl3.o: lib/htonl3.c lib/htonl3.h
	${CC} ${CFLAGS} -c lib/htonl3.c

label_mapping_table.o: lib/label_mapping_table.c lib/label_mapping_table.h
	${CC} ${CFLAGS} -c lib/label_mapping_table.c

larp_reply.o: lib/larp_reply.c lib/larp_reply.h
	${CC} ${CFLAGS} -c lib/larp_reply.c

process_pkt.o: lib/process_pkt.c lib/process_pkt.h
	${CC} ${CFLAGS} -c lib/process_pkt.c

clean: 
	rm server server.o $(OBJS)


