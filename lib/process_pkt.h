#include "server_def.h"
#include "debug.h"
#include "larp_def.h"

int check_larp_pkt(struct arphdr *ar_hdr);
int process_larp_req(void *buff, struct sockaddr_ll *addr);
