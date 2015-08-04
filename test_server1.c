/* You need to run the program as root user for the raw sockets to be created */
/* gcc-g -pthread test_server1.c -o server */

#include "server_def.h"
#include "process_pkt.h"
#include "larp_reply.h"

#if 0
struct ip_label_table
{
  uint32_t ipaddr;
  uint32_t label[11];
  int label_count;
  uint32_t timestamp;
  struct ip_label_table *next;
};
#endif

void print_struct()
{  
   struct label_stack l_stack;
   struct attr_tlv att_tlv;
   //printf("Sizeof type: %ld\n", sizeof(a_lst_tlv.type));
   //printf("Sizeof len: %ld\n", sizeof(a_lst_tlv.len));
   //printf("Sizeof label: %ld\n", sizeof(a_lst_tlv.label));
   //printf("Sizeof entropy: %ld\n", sizeof(a_lst_tlv.entropy));
   printf("Sizeof attr tlv: %ld\n", sizeof(att_tlv));
   printf("Sizeof label stack struct:%ld\n", sizeof(l_stack));
}

int main (int argc, char *argv[])
{
   int i;
   int sockfd; //listening socket
   fd_set rset; //for select
   int maxfdp; // for select
   pthread_t msg_thread_id;
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
   if(argc == 2){
      printf("Argc:%d, argv: %s\n", argc, argv[1]);
      if (strcmp (argv[1], "-d") != 0) {
         printf("Correct usage:./server -d\n");
         exit(1);
      }
      else if (strcmp (argv[1], "-d") == 0){
      //write code for -d option
      }
   }
   else if (argc > 2){
     printf("Correct usage:./server or ./server -d\n");
     exit(1);
   }
   //print_struct();
   /* Opening raw socket */
   sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
   if (sockfd < 0) { /* socket function returns negative value on error */
	printf("Receiving socket creation failed\n");
	exit (1);
   }
   //test_func(sockfd);
   #if 1
   for (;;)
   {
	FD_ZERO(&rset);
        FD_SET(sockfd, &rset);
        maxfdp = sockfd + 1;
	//printf("Entering select\n");
    
        /* select to wait on the PF_Socket*/
        if(select(maxfdp, &rset, NULL, NULL, NULL) < 0)
        {
		if (errno == EINTR) //If EINTR signal encountered, don't break
			continue;
		else {
			fprintf(stdout, "Select Error: %s\n", strerror(errno));
			exit (1);
		}
	}
	/* PF_PACKET socket is readable now*/
	if(FD_ISSET(sockfd, &rset))
	{
		/*code for creating a new thread and handling the recvfrom (error case also < 0 => error */

		for (i=0; i<1; i++) 
		if(pthread_create(&msg_thread_id, NULL, msg_recv, (void *) &sockfd) != 0) //non-zero is error
                {
                    perror("Message processing thread could not be created\n");
                    return 1;
                }
	}
   } 
   #endif
   //pthread_join(msg_thread_id, NULL);
}


void *msg_recv(void *sockfd)
{
    //printf("Inside thread\n");
    /*Detaching the thread*/
    if (pthread_detach(pthread_self()) != 0) {
	printf("Thread detach error\n");
	pthread_exit(NULL);
    }
    int recv_sockfd = *(int *) sockfd;
    ssize_t pkt_len; //length of the received packet
    struct sockaddr_ll send_addr; //To fill in the details of the received frame
    void *recvbuf = (void *) malloc (ETH_FRAME_LEN); //ETH_FRAME_LEN = 1514
    struct arphdr *ar_hdr;
    socklen_t from_len = sizeof(send_addr);
    bzero(&send_addr, sizeof(send_addr)); 
    pkt_len = recvfrom(recv_sockfd, recvbuf, ETH_FRAME_LEN, 0, (SA *) &send_addr, &from_len);
    if (pkt_len < 0) {
	perror("Recvfrom Error\n");
        pthread_exit(NULL);
    }
    printf("Received %zd bytes\n", pkt_len);
    printf("Received from interface index: %d\n", send_addr.sll_ifindex);
    process_larp_req(recvbuf, &send_addr);
    ar_hdr = (struct arphdr *) recvbuf;
    #if 0
    printf("SRC ip address: %u.%u.%u.%u\n", ar_hdr->ar_sip[0], ar_hdr->ar_sip[1], ar_hdr->ar_sip[2], ar_hdr->ar_sip[3]);
    #endif
    printf("ARP code: %u\n", ntohs(ar_hdr->ar_op));
    free(recvbuf);
    pthread_exit(NULL);
}

    
