#include "in_4_20_ma.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

int in_4_20_ma_read_thread(in_4_20_t* in_4_20);

int in_4_20_ma_init(int idx, in_4_20_t* in_4_20) {

    char filename[FILENAME_MAX_SIZE];

    // channel_idx
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_CHANNEL_IDX, idx);

    memset(in_4_20->channel_idx_filename, 0, sizeof(in_4_20->channel_idx_filename));
    strcpy(in_4_20->channel_idx_filename, filename);
    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    in_4_20->channel_idx = ret;

    // setting pressure low level error (mA)
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_PRESSURE_LOW_THRESHOLD, idx);
    float tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    in_4_20->pressure_low_threshold = tmp;
    in_4_20->pressure_low_threshold_raw = in_4_20_ma_convert_ma_to_raw(in_4_20->pressure_low_threshold);

    // setting pressure high level error (mA)
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_PRESSURE_HIGH_THRESHOLD, idx);
    tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    in_4_20->pressure_high_threshold = tmp;
    in_4_20->pressure_high_threshold_raw = in_4_20_ma_convert_ma_to_raw(in_4_20->pressure_high_threshold);

    // RAM memory value
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, VALUE_RAM_FILENAME, in_4_20->channel_idx);
    memset(in_4_20->ram_value_filename, 0, sizeof(in_4_20->ram_value_filename));
    strcpy(in_4_20->ram_value_filename, filename);
    printf("ram value filename: %s\r\n", in_4_20->ram_value_filename);

    // value
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, VALUE_FILENAME, in_4_20->channel_idx);

    memset(in_4_20->value_filename, 0, sizeof(in_4_20->value_filename));
    strcpy(in_4_20->value_filename, filename);

    ret = parse_integer_config(in_4_20->value_filename);
    in_4_20->value = ret;

    atomic_init(&in_4_20->value, 0);
    _Atomic int* value = (_Atomic int*)&in_4_20->value;
    atomic_store(value, ret);
//    pthread_create(&in_4_20->thread_id, NULL, in_4_20_ma_read_thread, in_4_20);

    if(ret < INPUT_NOT_CONNECTED_THRESHOLD_VALUE) {
        return -1;
    }
    return 0;
}

int in_4_20_ma_check_state(in_4_20_t* in_4_20)
{
	int val = atomic_load(&in_4_20->value);

	if(val < SWAPPED_WIRES_THRESHOLD_VALUE)
	{
		in_4_20->state = IN_4_20_SWAPPED_WIRES_ERROR;
	}
	else if(val < INPUT_NOT_CONNECTED_THRESHOLD_VALUE)
    {
    	in_4_20->state = IN_4_20_NOT_CONNECTED_ERROR;
    }
    else if (val < in_4_20->pressure_low_threshold_raw)
    {
    	in_4_20->state = IN_4_20_LOW_PRESSURE_ERROR;
    }
    else if (val > in_4_20->pressure_high_threshold_raw)
    {
    	in_4_20->state = IN_4_20_HIGH_PRESSURE_ERROR;
    }
    else if( (val > in_4_20->pressure_low_threshold_raw + RAW_PRESSURE_THRESHOLD_GAP) &&
    		 (val < in_4_20->pressure_high_threshold_raw - RAW_PRESSURE_THRESHOLD_GAP) )
    {
    	in_4_20->state = IN_4_20_NO_ERROR;
    }
    return in_4_20->state;
}

int in_4_20_ma_read(in_4_20_t* in_4_20)
{
	int val = atomic_load(&in_4_20->value);
    return val;
}

int in_4_20_ma_read_thread(in_4_20_t* in_4_20) {
	_Atomic int* value = (_Atomic int*)&in_4_20->value;
    while(1) {
        int ret = parse_integer_config(in_4_20->value_filename);
        atomic_store(value, ret);
        usleep(300000);
    }
}

int in_4_20_ma_read_thread_both(struct in_4_20_t**  in_4_20s) {
	int ret01 = 0;
	int ret02 = 0;
	int ret11 = 0;
	int ret12 = 0;
	in_4_20_t* in_4_20_left = in_4_20s[0];
	in_4_20_t* in_4_20_right = in_4_20s[1];
	_Atomic int* value_left = (_Atomic int*)&in_4_20_left->value;
	_Atomic int* value_right = (_Atomic int*)&in_4_20_right->value;
    while(1) {

    	ret01 = parse_integer_config(in_4_20_right->value_filename);
//    	ret01 = parse_integer_config("/sys/bus/iio/devices/iio:device0/in_voltage0_raw");
    	usleep(300000);
    	ret02 = parse_integer_config(in_4_20_right->value_filename);
//    	ret02 = parse_integer_config("/sys/bus/iio/devices/iio:device0/in_voltage0_raw");
    	atomic_store(value_right, ret02);
    	set_int_config(in_4_20_right->ram_value_filename, ret02);
        usleep(300000);

    	ret11 = parse_integer_config(in_4_20_left->value_filename);
//    	ret11 = parse_integer_config("/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
    	usleep(300000);
    	ret12 = parse_integer_config(in_4_20_left->value_filename);
//    	ret12 = parse_integer_config("/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
    	atomic_store(value_left, ret12);
    	set_int_config(in_4_20_left->ram_value_filename, ret12);
        usleep(300000);

//        printf("4-20: %d, **%d** and %d, **%d**\r\n", ret01, ret02, ret11, ret12);
        printf("4-20: %d and %d\r\n", ret02, ret12);

    }
}


float in_4_20_ma_convert_raw_to_ma(int value) {
	float res = (value - 2100) / 537.5 + 4.0;
	return res;
}

float in_4_20_ma_convert_ma_to_raw(float value) {
	int res = (value - 4.0) * 537.5 + 2100;
	return res;
}

