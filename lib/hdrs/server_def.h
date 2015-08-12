#ifndef SERVER_DEF_H
#define SERVER_DEF_H

#define MPLS_DATA 3

#include "parse_cmdline_args.h"
#define ARP_REQ_SIZE 46 /* size of ARP request */

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void * larp_req_recv(void *sockfd);
void *mpls_data_recv (void *sockfd);

#endif
