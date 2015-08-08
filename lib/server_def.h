#ifndef SERVER_DEF_H
#define SERVER_DEF_H

#include "parse_cmdline_args.h"
#define ARP_REQ_SIZE 46 /* size of ARP request */

void * msg_recv(void *sockfd);

#endif
