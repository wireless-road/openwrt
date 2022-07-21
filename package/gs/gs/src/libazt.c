#define AZTLIB
#include "libazt.h"


void azt_request_parser(char* rx_buf, int rx_buf_len);
static int azt_calculate_address(char* rx_buf, int starting_symbol_idx);
static int azt_validate_complimentary_bytes(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx);
static int azt_detect_command(char* rx_buf, int azt_request_cmd_idx, int ending_symbol_idx, char* azt_request_params, int* azt_request_params_cnt);

char azt_request_params[AZT_REQUEST_PARAMS_MAX_AMOUNT] = {0};
int azt_request_params_cnt = 0;

void azt_request_parser(char* rx_buf, int rx_buf_len) {
    for(int i = 0; i<rx_buf_len; i++) {
        printf("%02X ", rx_buf[i]);
    }
    printf("\n");

    // Check for starting byte idx
    int starting_symbol_idx = -1;
    for(int i = 0; i<rx_buf_len; i++) {
        if(rx_buf[i] == AZT_RQST_DEL_SYMBOL) {
            starting_symbol_idx = i+1;
            break;
        }
    }
    if(starting_symbol_idx == -1) { // No DEL symbol detected, skip the whole packet
        return;
    }

    // Check for ending byte idx
    int ending_symbol_idx = -1;
    for(int i = starting_symbol_idx+1; i<rx_buf_len; i++) {
        if((rx_buf[i] == AZT_RQST_ENDING_SYMBOL) && (rx_buf[i+1] == AZT_RQST_ENDING_SYMBOL)) {
            ending_symbol_idx = i;
            break;
        }
    }
    if(ending_symbol_idx == -1) {
        return;  // Ending symbols not found
    }

    int ret = azt_validate_complimentary_bytes(rx_buf, starting_symbol_idx+1, ending_symbol_idx);
    if(ret == -1) {
        return;  // Broken packet. Wrong complimentary bytes
    }
    int address = azt_calculate_address(rx_buf, starting_symbol_idx);
    if(address == -1) {
        return;
    }

//    if(address != TRK_DEFAULT_ADDRESS) {
//        return;  // not request for us
//    }
    printf("address: %d\n", address);

    int azt_request_cmd_idx = starting_symbol_idx+3;

    memset(azt_request_params, 0, AZT_REQUEST_PARAMS_MAX_AMOUNT);
    azt_request_params_cnt = 0;
    int azt_cmd = azt_detect_command(rx_buf, azt_request_cmd_idx, ending_symbol_idx, azt_request_params, &azt_request_params_cnt);

    printf("cmd: %02X. params: ", azt_cmd);
    for(int i=0; i<azt_request_params_cnt; i++) {
        printf("%02X ", azt_request_params[i]);
    }
    printf("\n");
}







int azt_init(void) {
    int ret = azt_port_init(azt_request_parser);
    if (ret == -1) {
        printf("Azt port initialization failed:  %d \n", ret);
        return -1;
    }
}

void azt_deinit(void) {
    azt_port_close();
}

void azt_handler(void) {
    azt_port_read();
}


//////////////////////////////////////////////////////////////////////////////////////
/////////// local functions  /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
static int azt_detect_command(char* rx_buf, int azt_request_cmd_idx, int ending_symbol_idx, char* azt_request_params, int* azt_request_params_cnt)
{
    int cmd = rx_buf[azt_request_cmd_idx];
    int j=0;
    for(int i=azt_request_cmd_idx+2; i<ending_symbol_idx; i+=2) {
        azt_request_params[j] = rx_buf[i];
        j++;
    }
    *azt_request_params_cnt = j;
    return cmd;
}

static int azt_calculate_address(char* rx_buf, int address_shift_byte_idx)
{
    char address_shift_code = rx_buf[address_shift_byte_idx];
    char related_address_code = rx_buf[address_shift_byte_idx+1];

    int address_shift = -1;
    for(int i=0; i<AZT_STARTING_SYMBOLS_AMOUNT; i++) {
        if(AZT_STARTING_SYMBOLS[i] == address_shift_code) {
            address_shift = AZT_ADDRESS_SHIFTS[i];
            break;
        }
    }
    if(address_shift == -1) {
        return -1;  // Wrong shift address byte
    }

    int related_address = -1;
    for(int i=0; i<AZT_RELATED_ADDRESSES_AMOUNT; i++) {
        if(AZT_RELATED_ADDRESS[i] == related_address_code) {
            related_address = AZT_RELATED_ADDRESS[i] - AZT_RELATED_ADDRESS_BASE;
            break;
        }
    }
    if(related_address == -1) {
        return -1;  // Wrong related address byte
    }

    int address = address_shift + related_address;
    return address;
}

static int azt_validate_complimentary_bytes(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx)
{
    for(int i=starting_symbol_idx; i<ending_symbol_idx; i+=2) {
//        printf("%02X ---> %02X\n", rx_buf[i], rx_buf[i+1] );
        if(rx_buf[i+1] != COMPLIMENTARY(rx_buf[i])) {
            printf("AZT. Complimentary error. %02X != %02X\n", rx_buf[i+1], COMPLIMENTARY(rx_buf[i]));
            return -1;
        }
    }
    return 0;
}