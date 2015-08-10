#include "htonl3.h"

extern int label_count;

static struct option long_options[] = {
	{"label_count", required_argument, NULL, 'l'},
	{"attr_tlv_en", no_argument, NULL, 'a'},
	{"attr_metric", required_argument, NULL, 'm'},
	{"entropy_flag_en", required_argument, NULL, 'e'},
	{"no_check", no_argument, NULL, 'n'}, 
	{"print_msgs", no_argument, NULL, 'p'},
	{"print_debugs", no_argument, NULL, 'd'}
  };

int parse_cmdline(int argc, char *argv[]);
void print_correct_usage();
