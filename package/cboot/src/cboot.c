#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/can.h>

#include "can_defs.h"

int verbose = 1;
int background = 0;
int local_port = 15731;
int destination_port = 15730;
unsigned char can_interface[4] = "can0";

char dev_state_str[3][30] = {"",
                             "device in app state",
                             "device in boot state"};



int CAN_socker_init(char* interface) {
    int sc;
    struct ifreq ifr;
    struct sockaddr_can caddr;
    socklen_t caddrlen = sizeof(caddr);

    memset(&caddr, 0, sizeof(caddr));
    strcpy(ifr.ifr_name, interface);

    if ((sc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        printf("CAN socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    caddr.can_family = AF_CAN;
    if (ioctl(sc, SIOCGIFINDEX, &ifr) < 0) {
        printf("CAN setup error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    caddr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sc, (struct sockaddr *)&caddr, caddrlen) < 0) {
        printf("CAN bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sc;
}

void print_verbose(char* direction, struct can_frame* frame, char* udpframe) {
    if (verbose && !background) {
        printf("%s CANID 0x%06X R", direction, frame->can_id);
        printf(" [%u]", *(udpframe+4));
        for (int i = 5; i < 5 + frame->can_dlc; i++) {
            printf(" %02x", *(udpframe+i));
        }
        printf("\n");
    }
}

void print_usage(char *prg) {
    printf("\nUsage: %s -l <port> -d <port> -i <can interface>\n", prg);
    printf("   Version 0.93\n");
    printf("\n");
    printf("         -l <port>           listening UDP port for the server - default 15731\n");
    printf("         -d <port>           destination UDP port for the server - default 15730\n");
    printf("         -i <can int>        can interface - default can0\n");
    printf("         -f                  running in foreground\n\n");
    printf("         -v                  verbose output (in foreground)\n\n");
}

void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "l:d:b:i:hfv?")) != -1) {
        switch (opt) {
            case 'l':
                local_port = strtoul(optarg, (char **)NULL, 10);
                break;
            case 'd':
                destination_port = strtoul(optarg, (char **)NULL, 10);
//                baddr.sin_port = htons(destination_port);
                break;
            case 'i':
                strncpy(can_interface, optarg, sizeof(can_interface) - 1);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'f':
                background = 0;
                break;
            case 'h':
            case '?':
                print_usage(basename(argv[0]));
                exit(EXIT_SUCCESS);
            default:
                printf("Unknown option %c\n", opt);
                print_usage(basename(argv[0]));
                exit(EXIT_FAILURE);
        }
    }
}

typedef enum{
	ST_BOOT_CONNECT = 0,
	ST_BOOT_SET_ERASE_ADDR,
	ST_BOOT_ERASE,
	ST_BOOT_SET_WRITE_ADDR,
	ST_BOOT_WRTE,
	ST_BOOT_SET_READ_ADDR,
	ST_BOOT_READ,
	ST_BOOT_VERIFY,
	ST_BOOT_EXIT,
}t_boot_state;

int32_t send_to_can(int can_sc, struct can_frame* frame)
{
	int32_t ret_val = 0;

	if (write(can_sc, frame, sizeof(*frame)) != sizeof(*frame))
	{
		printf("CAN write error: %s\n", strerror(errno));
		ret_val = -1;
	}else{
		printf("CAN snd OK\n");
	}

	return ret_val;
}

int32_t wait_can_response(int can_sc, struct can_frame* frame)
{
	int32_t ret_val = 0;
	fd_set readfds;

	/* Ждем не больше пяти секунд. */
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(can_sc, &readfds);

	int32_t res = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
	if(res)
	{
		if (FD_ISSET(can_sc, &readfds)) // received a CAN frame
		{
			if (read(can_sc, frame, sizeof(*frame)) < 0)
			{
				fprintf(stderr, "CAN read error: %s\n", strerror(errno));
				ret_val = -1;
			}else{
				ret_val = -1;
			}
		}else{
			ret_val = -1;
		}
	}else{
		ret_val = -1;
	}

	return ret_val;
}

int main(int argc, char **argv) {
    struct can_frame cframe;
    uint8_t rcv_data[8];
    int sc;		// CAN socket
    struct sockaddr_in saddr, baddr;
    uint32_t addr;

    printf("--------------------------------------------------------\n");
    printf("------ HELLO CAN bootloader for indicator app ----------\n");
    printf("--------------------------------------------------------\n");    

    //parse_args(argc, argv);

    sc = CAN_socker_init(can_interface);

    t_boot_state boot_state = ST_BOOT_CONNECT;
    while(boot_state != ST_BOOT_EXIT)
    {
		switch(boot_state)
		{
			case ST_BOOT_CONNECT:
				{
					cframe.can_id = CAN_ADDR + CMD_GET_STATE;
					cframe.can_dlc = 0;

					int32_t res = send_to_can(sc, &cframe);
					if(res == 0)
					{
						res = wait_can_response(sc, &cframe);
						if(res == 0)
						{
							printf("CAN rcv\n"
								   "    id = %u\n"
								   "    size = %u\n",
								   cframe.can_id, cframe.can_dlc);
							uint32_t addr = cframe.can_id;
							addr &= CAN_ADDR_MASK;
							if(addr == CAN_MASTER_ADDR)
							{
								uint32_t cmd = cframe.can_id & CAN_CMD_MASK;
								if(cmd == CMD_GET_STATE)
								{
									memcpy(rcv_data, cframe.data, 8);
									t_status_answer* p_answ = (t_status_answer*)rcv_data;
									if(p_answ->status == STATE_BOOTLOADER)
									{
										boot_state = ST_BOOT_SET_WRITE_ADDR;
										break;
									}
								}
							}
						}
					}
					boot_state = ST_BOOT_EXIT;
				}
				break;
			case ST_BOOT_SET_WRITE_ADDR:
				{
					cframe.can_id = CAN_ADDR + CMD_SET_ADDR;
					addr = START_APP_ADDR;
					memcpy(cframe.data, &addr, sizeof(addr));
					cframe.can_dlc = sizeof(addr);

					int32_t res = send_to_can(sc, &cframe);;
					if(res == 0)
					{
						res = wait_can_response(sc, &cframe);
						if(res == 0)
						{
							uint32_t addr = cframe.can_id;
							addr &= CAN_ADDR_MASK;
							if(addr == CAN_MASTER_ADDR)
							{
								uint32_t cmd = cframe.can_id & CAN_CMD_MASK;
								if(cmd == CMD_SET_ADDR)
								{
									memcpy(rcv_data, cframe.data, 8);
									t_std_answer* p_answ = (t_std_answer*)rcv_data;
									if(p_answ->_err == ERR_OK)
									{
										boot_state = ST_BOOT_ERASE;
										printf("now erase\n");
										break;
									}
								}
							}
						}
					}
					boot_state = ST_BOOT_EXIT;
				}
				break;
			case ST_BOOT_ERASE:
				do{
					cframe.can_id = CAN_ADDR + CMD_ERASE;
					cframe.can_dlc = 0;

					int32_t res = send_to_can(sc, &cframe);;
					if(res != 0)
					{
						boot_state = ST_BOOT_EXIT;
						break;
					}

					res = wait_can_response(sc, &cframe);

					if(res != 0)
					{
						boot_state = ST_BOOT_EXIT;
						break;
					}

					uint32_t addr = cframe.can_id;
					addr &= CAN_ADDR_MASK;
					if(addr != CAN_MASTER_ADDR)
					{
						boot_state = ST_BOOT_EXIT;
						break;
					}
					uint32_t cmd = cframe.can_id & CAN_CMD_MASK;
					if(cmd != CMD_ERASE)
					{
						boot_state = ST_BOOT_EXIT;
						break;
					}

					memcpy(rcv_data, cframe.data, 8);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err != ERR_OK)
					{
						boot_state = ST_BOOT_EXIT;
						break;
					}

					addr += ERASE_INC_ADDR;
				}while(addr < END_APP_ADDR);
				if(boot_state != ST_BOOT_EXIT)
				{
					boot_state = ST_BOOT_WRTE;
					printf("erase success\n");
					printf("now write\n");
				}
				break;
			case ST_BOOT_WRTE:
				break;
			case ST_BOOT_READ:
				break;
			case ST_BOOT_VERIFY:
				break;
		}
    }

    close(sc);
    return 0;
}
