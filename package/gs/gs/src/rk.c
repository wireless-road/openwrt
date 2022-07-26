//
// Created by al on 23.07.22.
//

#include "rk.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static int parse_true_false_config(char* filename);
static int parse_integer_config(char* filename);
static int azt_req_handler(azt_request_t* req, rk_t* self);

int rk_init(int idx, rk_t* rk) {
//    printf("RK %d. init\n", idx);

    char filename[FILENAME_MAX_SIZE];

    // isEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/isEnabled", idx);

    int ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->enabled = ret;
//    printf("RK enabled: %d\n", rk->enabled);

    // isLeft
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/isLeft", idx);

    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->side = ret;
//    printf("RK on left side: %d\n", rk->side);

    // address
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/address", idx);

    ret = parse_integer_config(filename);
    if((ret == -1) || (ret == 0)){
        return -1;
    }
    rk->address = ret;
//    printf("RK address: %d\n", rk->address);

    rk->azt_req_hndl = azt_req_handler;

    rk->state = trk_disabled_rk_installed;
    rk->state_issue = trk_state_issue_less_or_equal_dose;

    // isLeft
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, "/etc/gs/%d/isLocalControlEnabled", idx);

    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->local_control_allowed = ret;

    return 0;
}

static int azt_req_handler(azt_request_t* req, rk_t* self)
{
    if(req->address != self->address) {
        return 0;
    }

    char responce[AZT_RESPONCE_MAX_LENGTH] = {0};
    int cnt = 0;

    int higher_bits = 0;
    int lower_bits = 0;

    switch (req->cmd) {
        case AZT_REQUEST_TRK_STATUS_REQUEST:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_STATUS_REQUEST\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));

            responce[cnt] = self->state;
            cnt++;

            responce[cnt] = self->state_issue;
            cnt++;

            azt_tx(responce, cnt);
            break;
        case AZT_REQUEST_TRK_AUTHORIZATION:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_AUTHORIZATION\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_TRK_RESET:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_RESET\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_CURRENT_FUEL_DISCHARGE_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_CURRENT_FUEL_DISCHARGE_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FULL_FUEL_DISCHARGE_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_FULL_FUEL_DISCHARGE_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_SUMMATORS_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_SUMMATORS_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_TRK_TYPE:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_TYPE\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            responce[cnt] = AZT_TRK_TYPE;
            cnt++;
            azt_tx(responce, cnt);
            break;
        case AZT_REQUEST_RECORD_CONFIRMATION:
            printf("%s RK. Address %d. AZT_REQUEST_RECORD_CONFIRMATION\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_PROTOCOL_VERSION:
            printf("%s RK. Address %d. AZT_REQUEST_PROTOCOL_VERSION\n", self->side == left ? "Left" : "Right", self->address);

            cnt = 0;
            memset(responce, 0, sizeof(responce));

            int tmp = AZT_PROTOCOL_VERSION / 10000000 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 1000000 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 100000 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 10000 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 1000 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 100 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION / 10 % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            tmp = AZT_PROTOCOL_VERSION  % 10 + 0x30;
            responce[cnt] = tmp;
            cnt++;

            azt_tx(responce, cnt);
            break;
        case AZT_REQUEST_PRICE_PER_LITER_SETUP:
            printf("%s RK. Address %d. AZT_REQUEST_PRICE_PER_LITER_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP:
            printf("%s RK. Address %d. AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FUEL_DISCHARGE_DOSE_IN_RUBLES:
            printf("%s RK. Address %d. AZT_REQUEST_FUEL_DISCHARGE_DOSE_IN_RUBLES\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FUEL_DISCHARGE_DOSE_IN_LITERS:
            printf("%s RK. Address %d. AZT_REQUEST_FUEL_DISCHARGE_DOSE_IN_LITERS\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_UNCONDITIONAL_START:
            printf("%s RK. Address %d. AZT_REQUEST_UNCONDITIONAL_START\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_TRK_ADDRESS_CHANGE:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_ADDRESS_CHANGE\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_COMMON_PARAMETERS_SETUP:
            printf("%s RK. Address %d. AZT_REQUEST_COMMON_PARAMETERS_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_CURRENT_TRANSACTION:
            printf("%s RK. Address %d. AZT_REQUEST_CURRENT_TRANSACTION\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_READ_PARAMS:
            printf("%s RK. Address %d. AZT_REQUEST_READ_PARAMS\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            if(req->params_cnt == 0) {
                req->params_cnt = 2;
                req->params[0] = AZT_PARAM_TRK_MODE;
                req->params[1] = AZT_PARAM_TRK_DOSE_SETUP_TYPE;
            }
            for(int i=0; i<req->params_cnt; i++) {
//                printf("%02X param\n", req->params[i]);
                switch (req->params[i]) {
                    case AZT_PARAM_TRK_MODE:
                        // Param code
                        responce[cnt] = AZT_PARAM_TRK_MODE;
                        cnt++;

                        char trk_mode = self->side == left ? AZT_PARAM_TRK_MODE_VAL_LEFT : AZT_PARAM_TRK_MODE_VAL_RIGHT;

                        // Param value higher 4 bits
                        higher_bits = ((trk_mode & 0xF0) >> 4)  + 0x30;
                        responce[cnt] = higher_bits;
                        cnt++;

                        // Param value lower 4 bits
                        lower_bits = trk_mode & 0x0F + 0x30;
                        responce[cnt] = lower_bits;
                        cnt++;
                        break;
                    case AZT_PARAM_TRK_DOSE_SETUP_TYPE:
                        // Param code
                        responce[cnt] = AZT_PARAM_TRK_DOSE_SETUP_TYPE;
                        cnt++;

                        char trk_control_mode = self->local_control_allowed == TRUE ? AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_AND_LOCAL_CONTROL : AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_CONTROL_ONLY;

                        // Param value higher 4 bits
                        higher_bits = ((trk_control_mode & 0xF0) >> 4)  + 0x30;
                        responce[cnt] = higher_bits;
                        cnt++;

                        // Param value lower 4 bits
                        lower_bits = trk_control_mode & 0x0F + 0x30;
                        responce[cnt] = lower_bits;
                        cnt++;
                        break;
                    default:
                        break;
                }
            }
            if(cnt > 0) {
                azt_tx(responce, cnt);
            } else {
                azt_tx_nak();
            }
            break;
        case AZT_REQUEST_WRITE_PARAMS:
            printf("%s RK. Address %d. AZT_REQUEST_WRITE_PARAMS\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_CURRENT_DOSE_READING:
            printf("%s RK. Address %d. AZT_REQUEST_CURRENT_DOSE_READING\n", self->side == left ? "Left" : "Right", self->address);
            break;
        default:
            break;
    }
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