#include "in_4_20_ma.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

int in_4_20_ma_init(int idx, in_4_20_t* in_4_20) {

    char filename[FILENAME_MAX_SIZE];

    // channel_idx
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_CHANNEL_IDX, idx);

    memset(in_4_20->channel_idx_filename, 0, sizeof(in_4_20->channel_idx_filename));
    strcpy(in_4_20->channel_idx_filename, filename);
    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    in_4_20->channel_idx = ret;

    // value
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, VALUE_FILENAME, in_4_20->channel_idx);

    memset(in_4_20->value_filename, 0, sizeof(in_4_20->value_filename));
    strcpy(in_4_20->value_filename, filename);

//    ret = parse_integer_config(filename);
//    if(ret == -1) {
//        return -1;
//    }

    in_4_20->fd = open(in_4_20->value_filename, O_RDONLY | O_NONBLOCK);
    if (in_4_20->fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, in_4_20->value_filename, strerror(errno));
        return -1;
    }

    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    ret = read(in_4_20->fd, rx_buf, RX_BUF_SIZE);
    printf("4-20ma VALUE: %s\r\n", rx_buf);

    if(ret == -1) {
        printf("Error %i trying to read RK config file %s: %s\n", errno, in_4_20->value_filename, strerror(errno));
        return -1;
    }

    ret = strtol(rx_buf, NULL, 10);

    in_4_20->value = ret;

    if(ret < INPUT_NOT_CONNECTED_THRESHOLD_VALUE) {
        return -1;
    }
}

int in_4_20_ma_read(in_4_20_t* in_4_20) {

//    int ret = parse_integer_config(in_4_20->value_filename);

    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    int ret = read(in_4_20->fd, rx_buf, RX_BUF_SIZE);
    printf("4-20ma value: %s\r\n", rx_buf);
    if(ret == -1) {
        printf("Error %i trying to read RK config file %s: %s\n", errno, in_4_20->value_filename, strerror(errno));
        return -1;
    }

    ret = strtol(rx_buf, NULL, 10);

    if(ret == -1) {
        return -1;
    }
    in_4_20->value = ret;

    if(ret < INPUT_NOT_CONNECTED_THRESHOLD_VALUE) {
        return -1;
    }
//    return 0;
    return in_4_20->value;
}
