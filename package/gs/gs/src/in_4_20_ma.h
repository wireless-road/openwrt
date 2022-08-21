#ifndef IN_4_20_MA_H
#define IN_4_20_MA_H

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

#include "settings.h"

#include <pthread.h>

#define CONFIG_FILENAME_MAX_LENGTH      64

#define INPUT_NOT_CONNECTED_THRESHOLD_VALUE     100

typedef struct in_4_20_t in_4_20_t;

struct in_4_20_t {
    int fd;
    int channel_idx;
    int value;
    float value_converted;
    char channel_idx_filename[CONFIG_FILENAME_MAX_LENGTH];
    char value_filename[CONFIG_FILENAME_MAX_LENGTH];
    pthread_t thread_id;
};

#define VALUE_FILENAME                  "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"

#define DEF_CONFIG_FILE_CHANNEL_IDX              "/etc/gs/%d/in_4_20_channel_idx"

#define CONFIG_FILE_CHANNEL_IDX              "/mnt/gs/%d/in_4_20_channel_idx"

int in_4_20_ma_init(int idx, in_4_20_t* in_4_20);
int in_4_20_ma_read(in_4_20_t* in_4_20);

#endif  // IN_4_20_MA_H

