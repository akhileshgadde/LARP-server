/* You need to run the program as **root** user for the raw sockets to be created */
/* gcc-g -pthread test_server1.c -o server */

#include "lib/server_def.h"

int main (int argc, char *argv[])
{
   int i;
   int larp_sockfd; /* listening socket for LARP req and sending larp replies */
   int data_sockfd; /* To listen for MPLS data packets */
   int parse_ret;
   fd_set rset; //for select
   int maxfdp; // for select
   pthread_t larp_thread_id, data_thread_id;
   //printf("Size of struct: %ld\n", sizeof(struct ip_label_table));
   table_head = (struct ip_label_table *) calloc(1, sizeof(struct ip_label_table));
   table_head = NULL;
   #if 0
   if (pthread_mutex_init(&mutex_lock, NULL) != 0)
    {
        perror("Mutex initailization failed\n");
        return 1;
    }
  #endif
   #if 0
     	if(table_head == NULL)
   	printf("Table_head is NULL\n");
   #endif 
   /* Checking correct arguments given in command line */
   parse_ret = parse_cmdline(argc,argv);
   if(parse_ret == 1)
	printf("********No options provided. Using default values.**********\n"); 
   /* Opening raw socket for LARP requests and replies */
   larp_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
   if (larp_sockfd < 0) { /* socket function returns negative value on error */
	printf("LARP socket creation failed. Exiting...\n");
	exit (1);
   }
   data_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_MPLS_UC)); /* ETH_P_MPLS_UC	0x8847 for MPLS unicast traffic */
  if (data_sockfd < 0) { /* socket function returns negative value on error */
	printf("MPLS data traffic listening socket creation failed. Exiting...\n");
	exit (1);
  }
   //test_func(sockfd);
   for (;;)
   {
	FD_ZERO(&rset);
        FD_SET(larp_sockfd, &rset);
	FD_SET(data_sockfd, &rset);
        maxfdp = max (larp_sockfd, data_sockfd) + 1;
    
        /* select to wait on the PF_Socket*/
        if(select(maxfdp, &rset, NULL, NULL, NULL) < 0)
        {
		if (errno == EINTR) /* If EINTR signal encountered, don't break */
			continue;
		else {
			fprintf(stdout, "Select Error: %s\n", strerror(errno));
			exit (1);
		}
	}
	/* LARP socket is readable now*/
	if(FD_ISSET(larp_sockfd, &rset))
	{
            /*code for creating a new thread and handling the recvfrom (error case also < 0 => error */
	    if(pthread_create(&larp_thread_id, NULL, larp_req_recv, (void *) &larp_sockfd) != 0){ //non-zero is error
                  perror("LARP request thread creation error. Ignoring the packet.\n");
           }
	}

        /* MPLS data socket is readabale now */
	if(FD_ISSET(data_sockfd, &rset))
	{
		if(pthread_create(&data_thread_id, NULL, mpls_data_recv, (void *) &data_sockfd) != 0) {
		    perror("MPLS data thread creation error. Ignoring the packet\n");
		}    
        }
   } 
   //pthread_join(larp_thread_id, NULL);
}


void *larp_req_recv (void *sockfd)
{
    //printf("Inside thread\n");
    /*Detaching the thread*/
    if (pthread_detach(pthread_self()) != 0) {
	printf("LARP request thread detach error\n");
	pthread_exit(NULL);
    }
    int recv_sockfd = *(int *) sockfd;
    ssize_t pkt_len; //length of the received packet
    struct sockaddr_ll send_addr; /* To fill in the details of sender address */
    void *recvbuf = (void *) malloc (ETH_FRAME_LEN); //ETH_FRAME_LEN = 1514
    memset(recvbuf, 0, ETH_FRAME_LEN);
    struct arphdr *ar_hdr;
    socklen_t from_len = sizeof(send_addr);
    bzero(&send_addr, sizeof(send_addr)); 
    pkt_len = recvfrom(recv_sockfd, recvbuf, ETH_FRAME_LEN, 0, (SA *) &send_addr, &from_len);
    if (pkt_len < 0) { /* Empty packet */
	perror("Recvfrom Error: Empty packet\n");
        pthread_exit(NULL);
    }
    if (pkt_len != ARP_REQ_SIZE) { /* Checking ARP request size */
	perror("Recceived ARP request size is invalid. Ignoring..\n");
	pthread_exit(NULL);
    } 
    if (print_debugs)
    { 
       printf("LARP Req: Received %zd bytes ", pkt_len);
       printf("from interface index: %d\n", send_addr.sll_ifindex);
    }
    process_larp_req(recvbuf, &send_addr, send_addr.sll_ifindex, pkt_len);
    #if 0
    ar_hdr = (struct arphdr *) recvbuf;
    printf("SRC ip address: %u.%u.%u.%u\n", ar_hdr->ar_sip[0], ar_hdr->ar_sip[1], ar_hdr->ar_sip[2], ar_hdr->ar_sip[3]);
    printf("ARP code: %u\n", ntohs(ar_hdr->ar_op));
    #endif
    free(recvbuf);
    pthread_exit(NULL);
}

void *mpls_data_recv (void *sockfd)
{
    if (pthread_detach(pthread_self()) != 0) { 
       printf("MPLS data thread detach error\n"); 
       pthread_exit(NULL);
    }
    int data_sockfd = *(int *) sockfd;
    ssize_t pkt_len;
    struct sockaddr_ll send_addr;
    void *recvbuf = (void *) malloc (ETH_FRAME_LEN); //ETH_FRAME_LEN = 1514 
    memset(recvbuf, 0, ETH_FRAME_LEN);
    socklen_t from_len = sizeof(send_addr);
    bzero(&send_addr, sizeof(send_addr));
    pkt_len = recvfrom(data_sockfd, recvbuf, ETH_FRAME_LEN, 0, (SA *) &send_addr, &from_len);
    if (pkt_len < 0) { /* Empty packet */
        perror("Recvfrom Error:Empty packet\n"); 
        pthread_exit(NULL);                                                    
    }
    if (print_debugs)
    {
       printf("MPLS data packet: Received %zd bytes from interface index: %d\n", pkt_len, send_addr.sll_ifindex);
    }
    //process_mpls_data(recvbuf, &send_addr);
    free(recvbuf);
    pthread_exit(NULL);
}
