//
// Created by al on 23.07.22.
//

#include "rk.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static int azt_req_handler(azt_request_t* req, rk_t* self);
static int rk_is_not_fault();
static int rk_process();
static int rk_fueling_simulation(rk_t* self);
static void int_to_string_azt(int val, char* res, int* cnt);
//static int set_config(char* filename, char* data, int len);

char tmp[RX_BUF_SIZE] = {0};

int rk_init(int idx, rk_t* rk) {
    char filename[FILENAME_MAX_SIZE];


    // isEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_IS_ENABLED, idx);
    memset(rk->config_filename_is_enabled, 0, sizeof(rk->config_filename_is_enabled));
    strcpy(rk->config_filename_is_enabled, filename);
    int ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->enabled = ret;


    // isLeft
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_IS_LEFT, idx);
    memset(rk->config_filename_is_left, 0, sizeof(rk->config_filename_is_left));
    strcpy(rk->config_filename_is_left, filename);
    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->side = ret;


    // address
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_ADDRESS, idx);
    memset(rk->config_filename_address, 0, sizeof(rk->config_filename_address));
    strcpy(rk->config_filename_address, filename);
    ret = parse_integer_config(filename);
    if((ret == -1) || (ret == 0)){
        return -1;
    }
    rk->address = ret;


    rk->is_not_fault = rk_is_not_fault;
    rk->process = rk_process;
    rk->azt_req_hndl = azt_req_handler;
    rk->state = trk_disabled_rk_installed;
    rk->state_issue = trk_state_issue_less_or_equal_dose;


    // isLocalControlEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_IS_LOCAL_CONTROL_ENABLED, idx);
    memset(rk->config_filename_summator_price, 0, sizeof(rk->config_filename_is_local_control_enabled));
    strcpy(rk->config_filename_is_local_control_enabled, filename);
    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->local_control_allowed = ret;


    // summator_price
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_SUMMATOR_PRICE, idx);
    memset(rk->config_filename_summator_price, 0, sizeof(rk->config_filename_summator_price));
    strcpy(rk->config_filename_summator_price, filename);
    float tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->summator_price = tmp;


    // summator_volume
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_SUMMATOR_VOLUME, idx);
    memset(rk->config_filename_summator_volume, 0, sizeof(rk->config_filename_summator_volume));
    strcpy(rk->config_filename_summator_volume, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->summator_volume = tmp;


    // price_per_liter
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, DEF_CONFIG_FILE_PRICE_PER_LITER_MASK, idx);
    memset(rk->config_filename_price_per_liter, 0, sizeof(rk->config_filename_price_per_liter));
    strcpy(rk->config_filename_price_per_liter, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->fueling_price_per_liter = tmp;

    CAN_init(idx, &rk->can_bus);
//    rk->can_bus.transmit(&rk->can_bus, 15.0+idx, 10.0, (15.0 + idx)* 10.0);
    return 0;
}

static int rk_is_not_fault(rk_t* self) {
    // To-Do: implement errors checkout
    return TRUE;
}

static int rk_process(rk_t* self) {
    switch (self->state) {
        case trk_disabled_rk_installed:
            break;
        case trk_disabled_rk_taken_off:
            break;
        case trk_authorization_cmd:
            break;
        case trk_enabled_fueling_process:
            rk_fueling_simulation(self);
            break;
        case trk_disabled_fueling_finished:
            break;
        case trk_disabled_local_control_unit_dose:
            break;
        default:
            break;
    }
}

static int rk_fueling_simulation(rk_t* self) {
    static int store_prev_summators_flag = 0;
    usleep(100000);

    if(!store_prev_summators_flag) {
        self->prev_summator_volume = self->summator_volume;
        self->prev_summator_price = self->summator_price;
        store_prev_summators_flag = 1;
        self->fueling_current_finished_flag = FALSE;
    }

    if(self->fueling_dose_in_liters > 0.00) {

        // Simulation of fueling process 0.1 liters per second
        if(self->fueling_current_volume < self->fueling_dose_in_liters) {
            self->fueling_current_volume += 0.01;
            if(self->fueling_current_volume > self->fueling_dose_in_liters) {
                self->fueling_current_volume = self->fueling_dose_in_liters;
            }
        }
        // Simulation ends here

        if(fabs(self->fueling_current_volume - self->fueling_dose_in_liters) <= 0.001) {
            self->state = trk_disabled_fueling_finished;
            self->state_issue = trk_state_issue_less_or_equal_dose;
            store_prev_summators_flag = 0;
        }

        self->fueling_current_price = self->fueling_current_volume * self->fueling_price_per_liter;
        self->summator_volume = self->prev_summator_volume + self->fueling_current_volume;
        self->summator_price = self->prev_summator_price + self->fueling_current_price;

        self->can_bus.transmit(&self->can_bus, self->fueling_current_volume, self->fueling_price_per_liter, self->fueling_current_price);
        printf("currently fueled volume: %.2f of %.2f dose. summator volume: %f, summator price: %f\r\n", self->fueling_current_volume,
               self->fueling_dose_in_liters,
               self->summator_volume,
               self->summator_price);

        if(self->state == trk_disabled_fueling_finished) {
            char volume_summator[8] = {0};
            self->summator_volume += self->fueling_interrupted_volume;
            self->summator_price += self->fueling_interrupted_price;
            sprintf(volume_summator, "%.2f", self->summator_volume);
            set_config(self->config_filename_summator_volume, volume_summator, strlen(volume_summator));
            self->fueling_interrupted_volume = 0.00;
            self->fueling_interrupted_price = 0.00;
            self->fueling_current_finished_flag = TRUE;

            char price_summator[10] = {0};
            sprintf(price_summator, "%.2f", self->summator_price);
            set_config(self->config_filename_summator_price, price_summator, strlen(price_summator));
        }
    }
}

static int azt_req_handler(azt_request_t* req, rk_t* self)
{
    static reset_current_fueling_values_flag = FALSE;
    if(req->address != self->address) {
        return 0;
    }

    char responce[AZT_RESPONCE_MAX_LENGTH] = {0};
    int cnt = 0;

    int higher_bits = 0;
    int lower_bits = 0;

    int tmp;
    int ret;

    switch (req->cmd) {
        case AZT_REQUEST_TRK_STATUS_REQUEST:
//            printf("%s RK. Address %d. AZT_REQUEST_TRK_STATUS_REQUEST\n", self->side == left ? "Left" : "Right", self->address);
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
            ret = 0;  // To-Do: implement checkout whether we can start discharging
            if(tmp == 0) {
                self->state = trk_authorization_cmd;
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_TRK_RESET:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_RESET\n", self->side == left ? "Left" : "Right", self->address);
            ret = 0;  // To-Do: implement checkout whether we can start discharging
            // Возможные статусы ТРК до запроса: ‘2’ ,‘3’ или ‘8’
            // Возможные статусы ТРК после запроса: ‘4’ + ‘0’  или  ‘4’+’1’
            //									    ‘1’ или ‘0’

            if(self->state == trk_enabled_fueling_process) {
                self->fueling_interrupted_volume = self->summator_volume - self->prev_summator_volume;
                self->fueling_interrupted_price = self->summator_price - self->prev_summator_price;
            }
            if(ret == 0) {
                self->state = trk_disabled_fueling_finished;
                self->state_issue = trk_state_issue_less_or_equal_dose; // To-Do: implement correct issue setup
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_CURRENT_FUEL_CHARGE_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_CURRENT_FUEL_CHARGE_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FULL_FUELING_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_FULL_FUELING_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            self->fueling_current_price = self->fueling_current_volume * self->fueling_price_per_liter;

            char fueling_current_volume[VOLUME_DIGITS+1] = {0};
            sprintf(fueling_current_volume, "%07.2f", self->fueling_current_volume);
            fueling_current_volume[4] = fueling_current_volume[5];
            fueling_current_volume[5] = fueling_current_volume[6];
            fueling_current_volume[6] = 0x00;

            char fueling_current_price[PRICE_DIGITS+1] = {0};
            sprintf(fueling_current_price, "%09.2f", self->fueling_current_price);
            fueling_current_price[6] = fueling_current_price[7];
            fueling_current_price[7] = fueling_current_price[8];
            fueling_current_price[8] = 0x00;

            char fueling_price_per_liter_str[PRICE_PER_LITER_DIGITS+1] = {0};
            sprintf(fueling_price_per_liter_str, "%07.2f", self->fueling_price_per_liter);
            fueling_price_per_liter_str[4] = fueling_price_per_liter_str[5];
            fueling_price_per_liter_str[5] = fueling_price_per_liter_str[6];
            fueling_price_per_liter_str[6] = 0x00;

            strcpy(responce, fueling_current_volume);
            strcpy(responce + VOLUME_DIGITS, fueling_current_price);
            strcpy(responce + VOLUME_DIGITS + PRICE_DIGITS, fueling_price_per_liter_str );
            cnt = VOLUME_DIGITS + PRICE_DIGITS + PRICE_PER_LITER_DIGITS;

            azt_tx(responce, cnt);

            printf("\tfueling_current_volume: %.2f. fueling_current_price: %.2f\r\n", self->fueling_current_volume, self->fueling_current_price);
            if(reset_current_fueling_values_flag == TRUE) {
                reset_current_fueling_values_flag = FALSE;
//                printf("\treset_current_fueling_values_flag = %d\r\n", reset_current_fueling_values_flag);
                if(self->fueling_current_finished_flag == TRUE) {
                    self->fueling_current_volume = 0.00;
                    self->fueling_current_price = 0.00;
                }
            }
            break;
        case AZT_REQUEST_SUMMATORS_VALUE:
            printf("%s RK. Address %d. AZT_REQUEST_SUMMATORS_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            tmp = (int)roundf(self->summator_volume * 100.0);
            int_to_string_azt(tmp, responce, &cnt);
            tmp = (int)roundf(self->summator_price * 100.0);
            int_to_string_azt(tmp, responce, &cnt);
            azt_tx(responce, cnt);
            printf("\tsummator_volume: %.2f. summator_price: %.2f\r\n", self->summator_volume, self->summator_price);
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
            azt_tx_ack();
            self->state = trk_disabled_rk_installed;
//            self->state_issue = trk_state_issue_less_or_equal_dose;  // To-Do: implement state issue setup
//            azt_tx_can();  // To-Do: when it might responce as not ACK?
            reset_current_fueling_values_flag = TRUE;
//            printf("\treset_current_fueling_values_flag = %d\r\n", reset_current_fueling_values_flag);
            break;
        case AZT_REQUEST_PROTOCOL_VERSION:
            printf("%s RK. Address %d. AZT_REQUEST_PROTOCOL_VERSION\n", self->side == left ? "Left" : "Right", self->address);

            cnt = 0;
            memset(responce, 0, sizeof(responce));

            tmp = AZT_PROTOCOL_VERSION / 10000000 + 0x30;
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

            char price[6] = {0};
            price[0] = req->params[0];
            price[1] = req->params[1];
            price[2] = '.';
            price[3] = req->params[2];
            price[4] = req->params[3];
            float fueling_price_per_liter = strtof(price, NULL);
            self->fueling_price_per_liter = fueling_price_per_liter;
            tmp = set_config(self->config_filename_price_per_liter, price, strlen(price));
            if(tmp == 0) {
                azt_tx_ack();
            } else {
                azt_tx_can();
            }

            break;
        case AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP:
            printf("%s RK. Address %d. AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FUELING_DOSE_IN_RUBLES:
            printf("%s RK. Address %d. AZT_REQUEST_FUELING_DOSE_IN_RUBLES\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_FUELING_DOSE_IN_LITERS:
            printf("%s RK. Address %d. AZT_REQUEST_FUELING_DOSE_IN_LITERS\n", self->side == left ? "Left" : "Right", self->address);
            char volume[7] = {0};

            volume[0] = req->params[0];
            volume[1] = req->params[1];
            volume[2] = req->params[2];
            volume[3] = '.';
            volume[4] = req->params[3];
            volume[5] = req->params[4];
            float fueling_dose_in_liters = strtof(volume, NULL);
            self->fueling_dose_in_liters = fueling_dose_in_liters;
            self->fueling_dose_in_rubles = 0.00; // when workstation sets fuel dose volume we reset fuel dose in rubles

//            tmp = set_config(self->config_filename_price_per_liter, price, strlen(price));
            ret = 0;  // To-Do: implement checkout whether we can start fueling process
            if(ret == 0) {
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_UNCONDITIONAL_START:
            printf("%s RK. Address %d. AZT_REQUEST_UNCONDITIONAL_START\n", self->side == left ? "Left" : "Right", self->address);
            ret = 0;  // To-Do: implement checkout whether we can start discharging
//            Возможные статусы ТРК до запроса:     ‘2’  // trk_authorization_cmd
//            Возможные статусы ТРК после запроса:  ‘3’  // trk_enabled_fueling_process
            if(tmp == 0) {
                self->state = trk_enabled_fueling_process;
                self->fueling_current_volume = 0.00;
                self->fueling_current_price = 0.00;
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
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

static void int_to_string_azt(int val, char* res, int* cnt) {
    int div = 1000000000;
    for(int i=0; i<10; i++) {
        res[*cnt] = val / div + ASCII_ZERO;
//        printf("%d / %d -> %c. cnt: %d\n", val, div, res[*cnt], *cnt);
        if(res[*cnt] > ASCII_ZERO) {
            val = val % div;
        }
        div /= 10;
        (*cnt)++;
    }
}
