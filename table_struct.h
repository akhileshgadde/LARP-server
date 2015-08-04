#include "thread_mutex.h"
struct ip_label_table
{
  uint32_t ipaddr;
  uint32_t label[11];
  int label_count;
  uint32_t timestamp;
  uint32_t metric;
  struct ip_label_table *next;
};

/* Head for the ip-mpls label table*/
struct ip_label_table *table_head;

/*count for number of labels - given in command line by user */
static int label_count = 1;

/* count for number of attributes in ATTR_TLV */
static int attr_count = 1; 
