#include "label_mapping_table.h"

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
void u32fromu8 (uint8_t addr[IP_ADDR_SIZE], uint32_t *ip_u32)
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
struct ip_label_table* get_label_entry (uint8_t addr[IP_ADDR_SIZE])
{
  uint32_t ipaddr;
  char ip[INET_ADDRSTRLEN];
  struct ip_label_table *temp = table_head; /*table_head is initialized in main function */
  signal(SIGINT, sigintHandler);
  /*convert the ip address stored in uint8_t[4] to uint32_t */
  u32fromu8 (addr, &ipaddr); //ip address stored in ipaddr variable(32 bit)
  if(print_msgs) {
     printf("Searching entry for IP addr: %s\n",inet_ntop(AF_INET, &ipaddr, ip, sizeof(ip)));
  //print_ip_label_table();   
  }
  //pthread_mutex_lock(&mutex_lock);
  while (temp != NULL)
  {
     if(ipaddr == temp->ipaddr) { /* Ip address found in label table */
        if (print_msgs) {
           printf("Found entry for ip address in LARP table\n");
	}
        //pthread_mutex_unlock(&mutex_lock);
	return temp;
     }
     else
	temp = temp->next;
  }
  //pthread_mutex_unlock(&mutex_lock);
  /* creating a new node in the linked list with the ip-label mapping */
  if (print_msgs)
      printf("Creating new entry in lARP table\n");
  return (create_new_entry(ipaddr));
}

/* create a new node for the label table with the IP address, labels,etc */
struct ip_label_table* create_new_entry(uint32_t ipaddr)
{
  int i;
  signal(SIGINT, sigintHandler);
  struct ip_label_table *new_node = (struct ip_label_table *) malloc(sizeof(struct ip_label_table));
  new_node->label = (uint32_t *) malloc(label_count * sizeof(uint32_t));
  memset(new_node->label, 0, label_count * sizeof(uint32_t));
  struct ip_label_table *curr;
  new_node->next = NULL;
  /* fill in the values of the structure*/
  new_node->ipaddr = ipaddr;
  /* To generate a random label and metric */
  gen_label_metric(new_node);
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

/* Generate a random label entry using rand_r() */
void gen_label_metric(struct ip_label_table *new_node)
{
  int i;
  unsigned int seed = time(NULL) + (unsigned int) pthread_self(); /* seed for rand_r() */
  uint32_t l_label; /* variable to hold the random label generated and checked for any overlap */
  if (label_range_en == 0) /* No label range specified by user. Use default values */
  {
     label_range_min = LABEL_MIN;
     label_range_max = LABEL_MAX;
  }
    /* To generate a random label between 16 and (2^20-1) */
    for (i = 0; i < label_count; i++)
    {
       again:
        l_label = label_range_min + rand_r(&seed) / (RAND_MAX / (label_range_max - label_range_min + 1) + 1);
        if (find_dup_label(l_label) != 0) /* check for any duplication of the generated label */
	goto again;
        new_node->label[i] = l_label;
    } 
  if (attr_tlv_flag == 1) {
     //printf("Attr_metric: %u\n", attr_metric);
     if (attr_metric == 0) { /* No Metric value provided by user, generating a random metric */
    /* Generate a random metric value between 1 to (2^20-1) */
    new_node->metric = METRIC_MIN + rand_r(&seed) / (RAND_MAX / (METRIC_MAX - METRIC_MIN + 1) + 1);
     }
    else { /* store metric given by user for all entries */ 
    if (print_msgs)
	    printf("ATTR_METRIC in create_new_node: %u\n", attr_metric);
    new_node->metric = attr_metric;
    }
  }
  else /*attr_tlv_flag = 0 */
	attr_metric = 0;
}

/*To check if the generated label already exists in the table */
int find_dup_label(uint32_t l_label)
{
  int i;
  struct ip_label_table *curr = table_head;
  signal(SIGINT, sigintHandler);
  if(table_head == NULL)
	return 0;
  while (curr != NULL)
  {
      for (i=0; i<label_count; i++) {
          if (curr->label[i] == l_label)
	      return 1;
      }
      curr = curr->next;
   }
   return 0;
}

/* search and find the label(s) for given IP address */
uint32_t* find_label(uint32_t ipaddr)
{
  struct ip_label_table *tmp = table_head;
  signal(SIGINT, sigintHandler);
  if (table_head == NULL)
     return NULL;
  while (tmp != NULL)  
  {
       if (ipaddr == tmp->ipaddr)	/* found Ip addr in table */
	  return tmp->label;
       tmp = tmp->next;
  }
  return NULL; /* No entry found */
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

/* Delete the entery for Ip address in label mapping table */
int delete_label_entry(uint32_t ipaddr)
{
  struct ip_label_table *curr = table_head;
  struct ip_label_table *tmp; /* To store the entry to be deleted */
  struct ip_label_table *prev = table_head; /* To store the previous node to the node to be deleted */
  if (curr == NULL) /* Table is empty */
	return 1;
  while (curr != NULL)
  {
     if(curr->ipaddr == ipaddr)
     {
	tmp = curr;
        if(prev == table_head) /* only one node in the table */
		prev = tmp->next;
	else
	        prev->next = tmp->next;
	free(tmp);
	return 0;
     }
     prev = curr;
     curr = curr->next;
  }
  return 1; /* no matching entry found */
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
