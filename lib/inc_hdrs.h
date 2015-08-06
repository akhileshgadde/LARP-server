#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
//#include <linux/if_arp.h> //#define ARPHRD_SLIP     256
#include <pthread.h>
#include <inttypes.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

/* Structures sizes */
#define ARP_HDR_SIZE 28
#define TYPE_LEN_SIZE 2
#define LABEL_STACK_SIZE 3
#define ATTR_TLV_SIZE 4

/*other header sizes */
#define ETH_HDR_SIZE 14 /* size of ETH header - DMAC + SMAC + TYPE */
#define IP_ADDR_SIZE 4 /*size of IPv4 address */
#define IPV6_ADDR_SIZE 16
#define LARP_REPLY_OP 2
#define TLV_LST_TYPE 1
#define TLV_ATTR_TYPE 2
#define DEFAULT_METRIC 10

#if 0
/* variable for pthread mutexes */
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

typedef struct sockaddr SA;
