CC = gcc

LIBS = -lpthread\

FLAGS = -g -O2
CFLAGS = ${FLAGS} -I -Wall

HDRS_PATH = lib/hdrs
SRC_PATH  = lib/src
OBJ_PATH  = lib/obj  

OBJS = lib/obj/htonl3.o lib/obj/label_mapping_table.o lib/obj/larp_reply.o lib/obj/process_data_pkt.o lib/obj/parse_cmdline_args.o lib/obj/process_process_larp_req.o lib/obj/ioctl_fns.o lib/obj/hexdump.o

all: server

server: lib/obj/larp_server.o $(OBJS)
	${CC} ${FLAGS} -o server lib/obj/larp_server.o $(OBJS) ${LIBS}

lib/obj/larp_server.o: larp_server.c $(OBJS)
	${CC} ${CFLAGS} -o lib/obj/larp_server.o -c larp_server.c ${LIBS}

lib/obj/htonl3.o: lib/src/htonl3.c lib/hdrs/htonl3.h
	${CC} ${CFLAGS} -o lib/obj/htonl3.o -c lib/src/htonl3.c

lib/obj/label_mapping_table.o: lib/src/label_mapping_table.c lib/hdrs/label_mapping_table.h
	${CC} ${CFLAGS} -o lib/obj/label_mapping_table.o -c lib/src/label_mapping_table.c

lib/obj/larp_reply.o: lib/src/larp_reply.c lib/hdrs/larp_reply.h
	${CC} ${CFLAGS}  -o lib/obj/larp_reply.o -c lib/src/larp_reply.c

lib/obj/process_process_larp_req.o: lib/src/process_larp_req.c lib/hdrs/process_larp_req.h
	${CC} ${CFLAGS}  -o lib/obj/process_process_larp_req.o -c lib/src/process_larp_req.c

lib/obj/parse_cmdline_args.o: lib/src/parse_cmdline_args.c lib/hdrs/parse_cmdline_args.h
	${CC} ${CFLAGS}  -o lib/obj/parse_cmdline_args.o -c lib/src/parse_cmdline_args.c

lib/obj/process_data_pkt.o: lib/src/process_data_pkt.c lib/hdrs/process_data_pkt.h
	${CC} ${CFLAGS}  -o lib/obj/process_data_pkt.o -c lib/src/process_data_pkt.c

lib/obj/ioctl_fns.o: lib/src/ioctl_fns.c lib/hdrs/ioctl_fns.h
	${CC} ${CFLAGS}  -o lib/obj/ioctl_fns.o -c lib/src/ioctl_fns.c

lib/obj/hexdump.o: lib/src/hexdump.c lib/hdrs/hexdump.h
	${CC} ${CFLAGS}  -o lib/obj/hexdump.o -c lib/src/hexdump.c

clean: 
	rm server lib/obj/larp_server.o $(OBJS)


