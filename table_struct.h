#include "thread_mutex.h"
#define LABEL_MAX 1048575 /* 2^20 - 1 */
#define LABEL_MIN 16 /* lowest unspecial label allowed */
#define METRIC_MAX 1048575 /* 2^20 - 1 */
#define METRIC_MIN 1 /* metric starting value */ 
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
