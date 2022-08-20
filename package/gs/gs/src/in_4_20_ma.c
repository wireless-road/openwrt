#include "in_4_20_ma.h"

#define FILENAME_MAX_SIZE   64

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

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    in_4_20->value = ret;

    if(ret < INPUT_NOT_CONNECTED_THRESHOLD_VALUE) {
        return -1;
    }
}

int in_4_20_ma_read(in_4_20_t* in_4_20) {

    int ret = parse_integer_config(in_4_20->value_filename);
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
