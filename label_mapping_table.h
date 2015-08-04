#include "inc_hdrs.h"
#include "debug.h"
#include "table_struct.h"
//#include "thread_mutex.h"

void u32fromu8 (uint8_t addr[4], uint32_t *ip_u32);
uint32_t get_ts();
struct ip_label_table* get_label_map (uint8_t addr[4]);
struct ip_label_table* create_new_entry(uint32_t ipaddr);
struct ip_label_table* find_last_node ();
void print_ip_label_table();
void sigintHandler(int sig_num);
void free_alloc_mem();

/* Signal handler for SIGINT */
void sigintHandler(int sig_num)
{
  /*Reset handler to catch SIGINT next time */
  signal(SIGINT, sigintHandler);
  free_alloc_mem();
  printf("\nFreed allocated memory before exiting the program\n");
  exit(0);
}

/* Function to free allocated memory before exiting*/
void free_alloc_mem()
{
   struct ip_label_table *curr = table_head;
   struct ip_label_table *tmp;
   while(curr != NULL)
   {
      tmp = curr;
      curr = curr->next;
      free(tmp);
   }
}

/* Function to convert ip address stored in uint8_t[4] to uint32_t */
void u32fromu8 (uint8_t addr[4], uint32_t *ip_u32)
{
    *ip_u32 = addr[0] |  (addr[1] << 8) | (addr[2] << 16) | (addr[3] << 24);
}

/* To get the current unix timestamp in seconds */
uint32_t get_ts()
{
   struct timeval tv;
   uint32_t ts; 
   if (gettimeofday(&tv, NULL) != 0) /* 0 on success or -1 on failure */
   {
	perror("gettimeofday:unable to get time\n");
	exit (1);
   }
   /* ignore the microseconds and consider only the seconds */
   ts = (tv.tv_sec);
   return ts;
}

/* checks if IP address is in table else creates a new entry. Returns the address of the node where IP is present */
struct ip_label_table* get_label_map (uint8_t addr[4])
{
  uint32_t ipaddr;
  char ip[INET_ADDRSTRLEN];
  struct ip_label_table *temp = table_head; /*table_head is initialized in main function */
  signal(SIGINT, sigintHandler);
  /*convert the ip address stored in uint8_t[4] to uint32_t */
  u32fromu8 (addr, &ipaddr); //ip address stored in ipaddr variable(32 bit)
  #if PRINT
  printf("Searching entry for IP addr: %s\n",inet_ntop(AF_INET, &ipaddr, ip, sizeof(ip)));
  //print_ip_label_table();   
  #endif
  //pthread_mutex_lock(&mutex_lock);
  while (temp != NULL)
  {
     if(ipaddr == temp->ipaddr) { /* Ip address found in label table */
        #if PRINT
	printf("Found entry for ip address in LARP table\n");
	#endif
        //pthread_mutex_unlock(&mutex_lock);
	return temp;
     }
     else
	temp = temp->next;
  }
  //pthread_mutex_unlock(&mutex_lock);
  /* creating a new node in the linked list with the ip-label mapping */
  printf("Creating new entry in lARP table\n");
  return (create_new_entry(ipaddr));
}

/* create a new node for the label table with the IP address, labels,etc */
struct ip_label_table* create_new_entry(uint32_t ipaddr)
{
  int i;
  signal(SIGINT, sigintHandler);
  struct ip_label_table *new_node = (struct ip_label_table *) malloc(sizeof(struct ip_label_table));
  struct ip_label_table *curr;
  new_node->next = NULL;
  /* fill in the values of the structure*/
  new_node->ipaddr = ipaddr;
  /* To generate a random label between 16 and (2^20-1) */
  for (i = 0; i<label_count; i++)
  {
     /* Need to add check if already label exists for another entry in table - coce to be added */
     new_node->label[i] = 16 + rand() / (RAND_MAX / (1048575 - 16 + 1) + 1);
  }  
  /* Generate a random metric value between 1 to 255 */
  new_node->metric = 1 + rand () / (RAND_MAX / (255 - 1 + 1) + 1);
  /* Fill timetsamp in seconds */
  new_node->timestamp = get_ts();
  /* link the newly created node to the table*/
  curr = find_last_node();
  //pthread_mutex_lock(&mutex_lock);
  if (curr == NULL) {/* No entry in table */
	curr = new_node;
	table_head = new_node; /*updating head pointer */
  }
  else /* non empty case */
        curr->next = new_node;
  //pthread_mutex_unlock(&mutex_lock);
  return new_node;
}

/* search and return the address of the last node in the table */
struct ip_label_table* find_last_node ()
{
  struct ip_label_table *curr = table_head;
  signal(SIGINT, sigintHandler);
  if (table_head == NULL) /* No entry in the table */
      return table_head;
  //pthread_mutex_lock(&mutex_lock);
  while(curr->next != NULL)
  {
     curr = curr->next;
  }
  //pthread_mutex_unlock(&mutex_lock);
  return curr;
}

/* Funtion to print the LARP table */
void print_ip_label_table()
{
  signal(SIGINT, sigintHandler);
  struct ip_label_table *tmp = table_head;
  char ip[INET_ADDRSTRLEN];
  if (table_head == NULL) {
	printf("LARP table is empty\n");
	return;
  }
  else {
     printf("****************START******************\n");
     while (tmp != NULL)
     {
        printf("Ip address\t label_count\t Label(s)\t timestamp\n");
	/*if(ip == 0) {
		perror("INET_PTON error\n");
		return;
	}*/
	printf("%s\t %d\t %u\t %u\n", inet_ntop(AF_INET, &tmp->ipaddr, ip, sizeof(ip)), label_count, tmp->label[0],tmp->timestamp);   
     }
     printf("*****************END********************\n");
  }
}
