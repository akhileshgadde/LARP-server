CC = gcc

LIBS = -lpthread\

FLAGS = -g -O2
CFLAGS = ${FLAGS} -I -Wall

OBJS = htonl3.o label_mapping_table.o larp_reply.o process_pkt.o 

all: server

server: larp_server.o $(OBJS)
	${CC} ${FLAGS} -o server larp_server.o $(OBJS) ${LIBS}

larp_server.o: larp_server.c $(OBJS)
	${CC} ${CFLAGS} -o larp_server.o -c larp_server.c ${LIBS}

htonl3.o: lib/htonl3.c lib/htonl3.h
	${CC} ${CFLAGS} -c lib/htonl3.c

label_mapping_table.o: lib/label_mapping_table.c lib/label_mapping_table.h
	${CC} ${CFLAGS} -c lib/label_mapping_table.c

larp_reply.o: lib/larp_reply.c lib/larp_reply.h
	${CC} ${CFLAGS} -c lib/larp_reply.c

process_pkt.o: lib/process_pkt.c lib/process_pkt.h
	${CC} ${CFLAGS} -c lib/process_pkt.c

clean: 
	rm server larp_server.o $(OBJS)


