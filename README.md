# LARP-server
Implementation of Labelled ARP protocol server application in userspace using Linux PF_PACKET SOCK_DGRAM and SOCK_RAW sockets.

Usage:
------

$ make clean

$ make all

$ ./server [-n] [-p] [-d] [-h] [-a] [-m num] [-l num] [-r min,max] [-e 1/0]	

                                          OR 

$ ./server [--no_check] [--print_msgs] [--print_debugs] [--print_hex_dump] [--attr_tlv] [--attr_metric num] [--label_count num] [--label_range min,max] [--entropy_flag 1/0]
-----------------------------------------------------------------------------------------------------------------------------

The server application can be run using the below options:

•	-n / --no_check : for testing server without a client. It responds with LARP reply messages for every ARP message received.

•	-p / --print_msgs: print additional messages to show the flow of programs(for debugging)

•	-d / --print_debugs: print debug messages. 

•	-h / -- print_hex_dump: print the hexdump of the packets. (LARP request, LARP reply and MPLS data packet)

•	-a / --attr_tlv: Enable attribute TLV for the outgoing LARP replies.

•	-m [num] / --attr_metric [num]: Manually provide a metric value in the TLV-ATT. Else, a randomly generated value is used in LARP replies

•	-l [num] / --label_count [num]: Number of labels to be stacked in the TLV-LST. The number specifies the label count in the outgoing LARP reply label stack TLV.

•	-r [min,max] / -- label_range [min,max]: Specify the range from which LARP labels need to be generated for sending out in LARP replies. If this option is not used, a randomly generated label value(s) between 16 and (2^20-1) is used.

•	-e [1/0] / -- entropy_flag [1/0]: Enable/disable the entropy flag in the TLV_LST. 1 is enable and o is disable. Disabled by default.
