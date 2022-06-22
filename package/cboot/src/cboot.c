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

// 11 standart id
//  id bit number        10 9 8 7 6 5    4 3 2 1 0
//  id bit structure   |   dev addr   |  cmd code  |


#define CAN_ADDR_BIT_POSITION		(5UL)
#define CAN_ADDR_MASK				(0x3FUL << CAN_ADDR_BIT_POSITION)
#define CAN_ADDR					(1UL << CAN_ADDR_BIT_POSITION)

#define CAN_MASTER_ADDR				(0UL << CAN_ADDR_BIT_POSITION)

#define CAN_CMD_MASK				(0x1FUL)

#define CMD_SET_HI_DATA				(0UL)
#define CMD_SET_LO_DATA				(1UL)
#define CMD_FW_UPDATE				(2UL)
#define CMD_SET_ADDR				(3UL)
#define CMD_ERASE					(4UL)
#define CMD_WRITE					(5UL)
#define CMD_READ					(6UL)
#define CMD_GO_APP					(7UL)
#define CMD_GET_STATE				(8UL)


#define	STATE_APP					(1UL)
#define	STATE_BOOTLOADER			(2UL)

#define ERR_OK			(0UL)
#define ERR_ERR			(0xFFUL)

#define RESTART_DELAY	(40UL)
#define	BOOT_WR_LEN		(2UL)
#define BOOT_RD_LEN		(2UL)

#define GO_APP_TIMEOUT	(40UL)
#define RESTART_TIMEOUT	(40UL)

typedef struct {
	uint8_t status;
	uint32_t build_date;
	uint16_t build_number;
}__attribute__((packed)) t_status_answer;

typedef struct {
	uint32_t address;
}__attribute__((packed)) t_set_address_cmd;

typedef struct {
	uint8_t _err;
}__attribute__((packed)) t_std_answer;

typedef struct {
	uint32_t data[2];
}__attribute__((packed)) t_write_cmd;

typedef struct {
	uint32_t data[2];
}__attribute__((packed)) t_read_answer;

typedef struct{
	uint8_t data[8];
	uint32_t len;
	uint32_t id;
}t_rcv_data;

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

int main(int argc, char **argv) {
    struct can_frame frame;
    int sc;		// CAN socket
    struct sockaddr_in saddr, baddr;
    fd_set readfds;

    printf("--------------------------------------------------------\n");
    printf("------ HELLO CAN bootloader for indicator app ----------\n");
    printf("--------------------------------------------------------\n");    

    //parse_args(argc, argv);

    sc = CAN_socker_init(can_interface);

    printf("sc: %d\n", sc);
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sc, &readfds);

        frame.can_id = CAN_ADDR + CMD_GET_STATE;
        frame.can_dlc = 0;

        if (write(sc, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame))
        {
            printf("CAN write error: %s\n", strerror(errno));
            return -1;
        }else{
            printf("CAN snd OK\n");
        }

        if( select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
        {
            printf("SELECT error: %s\n", strerror(errno));
            return -1;
        }

        // received a CAN frame
        if (FD_ISSET(sc, &readfds)) {
            if (read(sc, &frame, sizeof(struct can_frame)) < 0) {
                fprintf(stderr, "CAN read error: %s\n", strerror(errno));
            }
            else {
                printf("CAN rcv\n"
                       "    id = %u\n"
                       "    size = %u\n",
                       frame.can_id, frame.can_dlc);
                uint32_t addr = frame.can_id;
                addr &= CAN_ADDR_MASK;
                if(addr == CAN_MASTER_ADDR) 
                {
                    uint32_t cmd = frame.can_id & CAN_CMD_MASK;
                    switch(cmd)
                    {
                        case CMD_GET_STATE:
                            {
                                t_status_answer answ;
                                memcpy(&answ, &frame.data, frame.can_dlc);
                                if((answ.status == 0) || (answ.status > 2))
                                {
                                    printf("DEV status - unknow\n");
                                }else{
                                    printf("DEV status - %s\n", dev_state_str[answ.status]);
                                }
                                printf("DEV_FW build data - %u\n", answ.build_date);
                                printf("DEV_FW build number - %u\n", answ.build_number);
                            }
                            break;
                    }
                }
            }
        }

        sleep(1);

    }
    close(sc);
    return 0;
}
