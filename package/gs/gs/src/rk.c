//
// Created by al on 23.07.22.
//

#include "rk.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static int parse_true_false_config(char* filename);
static int parse_integer_config(char* filename);

int rk_init(int idx, rk_t* rk) {
    printf("RK %d. init\n", idx);

    char filename[FILENAME_MAX_SIZE];

    // isEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/isEnabled", idx);

    int ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->enabled = ret;
    printf("RK enabled: %d\n", rk->enabled);

    // isLeft
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/isLeft", idx);

    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->side = ret;
    printf("RK on left side: %d\n", rk->side);

    // address
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/address", idx);

    ret = parse_integer_config(filename);
    if((ret == -1) || (ret == 0)){
        return -1;
    }
    rk->address = ret;
    printf("RK address: %d\n", rk->address);

    return 0;
}


static int parse_true_false_config(char* filename) {
    int fd;
    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = read(fd, rx_buf, 1);
    close(fd);
    if(ret != 1) {
        printf("Error trying to read RK config file %s. Expected 1 byte. Gotten %d bytes: %s\n", filename, ret, rx_buf);
        return -1;
    }

    int isEnabled = DIGIT_CHAR_TO_NUM(rx_buf[0]);

    if((isEnabled == TRUE) || (isEnabled == FALSE)) {
        return isEnabled;
    } else {
        printf("Error unknown config param in %s: %d\n", filename, isEnabled);
        return -1;
    }

}


static int parse_integer_config(char* filename) {
    int fd;
    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = read(fd, rx_buf, RX_BUF_SIZE);
    close(fd);
    if(ret == -1) {
        printf("Error %i trying to read RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    char* ptr;
    int res = strtol(rx_buf, &ptr, 10);

    return res;
}