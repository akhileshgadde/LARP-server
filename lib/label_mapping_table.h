#include "inc_hdrs.h"
#include "debug.h"

#define LABEL_MAX 1048575 /* 2^20 - 1 */
#define LABEL_MIN 16 /* lowest unspecial label allowed */
#define METRIC_MAX 1048575 /* 2^20 - 1 */
#define METRIC_MIN 1 /* metric starting value */ 

/* structure to hold the ip-label amppping entries */
struct ip_label_table
{
  uint32_t ipaddr;
  uint32_t *label; /* memory would be allocated based on number of labels */
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

/* entropy flag */
#define ENTROPY_DEF 1

void u32fromu8 (uint8_t addr[IP_ADDR_SIZE], uint32_t *ip_u32);
uint32_t get_ts();
struct ip_label_table* get_label_entry (uint8_t addr[IP_ADDR_SIZE]);
struct ip_label_table* create_new_entry(uint32_t ipaddr);
struct ip_label_table* find_last_node ();
void print_ip_label_table();
void sigintHandler(int sig_num);
void free_alloc_mem();
int find_dup_label(uint32_t l_label);
void gen_label_metric(struct ip_label_table *new_node);
int delete_label_entry(uint32_t ipaddr);
