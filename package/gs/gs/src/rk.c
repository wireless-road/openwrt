//
// Created by al on 23.07.22.
//

#include "rk.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static int azt_req_handler(azt_request_t* req, rk_t* self);
static int rk_is_not_fault();
static int rk_process(rk_t* self);
static int rk_stop(rk_t* self);
static int rk_fueling_scheduler(rk_t* self);
static void int_to_string_azt(int val, char* res, int* cnt, int len);
static void rk_indicate_error_message(rk_t* self);
static int rk_check_state(rk_t* self);
static void button_start_callback(rk_t* self, int code);
static void button_stop_callback(rk_t* self, int code);
static void button_start_released_callback(rk_t* self, int code);
static void button_stop_released_callback(rk_t* self, int code);
static void button_start_handler(rk_t* self);
static void rk_store_starting_values(rk_t* self);
static void rk_start_fueling_process(rk_t* self);
static void rk_start_local_fueling_process(rk_t* self);
static void rk_stop_fueling_process(rk_t* self, int* cnt);
static int rk_fueling_log_results(rk_t* self);

static void valve_low_on(rk_t* self);
static void valve_middle_on(rk_t* self);
static void valve_high_on(rk_t* self);
// Нет отдельных функций для отключения каждой линии по отдельности,
// поскольку включении каждой следующей линии подразумевает отключение всех линий, которые должны быть выключены,
// а имеено, включение третьей линии автоматически означает выключение второй линии
static void valve_all_off(rk_t* self);

//static int valves_amount = 1;  // To-Do: implement config file and web interface

char tmp[RX_BUF_SIZE] = {0};

int rk_init(int idx, rk_t* rk) {
    char filename[FILENAME_MAX_SIZE];

    rk->state = trk_disabled_rk_installed;
    rk->cnt = 0;
    rk->store_prev_summators_flag = 0;

    rk->btn_clbk_start = button_start_callback;
    rk->btn_clbk_stop = button_stop_callback;
    rk->btn_clbk_start_released = button_start_released_callback;
    rk->btn_clbk_stop_released = button_stop_released_callback;
    rk->start_button_clicked_flag = 0;
    rk->start_button_delay_cnt = 0;
    rk->stop_button_clicked_flag = 0;
    rk->start_button_currently_pressed_flag = 0;
    rk->stop_button_currently_pressed_flag = 0;
    rk->reset_command_received_flag = 0;

    rk->fueling_current_volume = 0.00;
    rk->fueling_interrupted_volume = 0.00;
    rk->fueling_current_price = 0.00;
    rk->fueling_interrupted_price = 0.00;

    rk->current_valve_number = 0.00;

    rk->in_4_20.enabled = 0;
    rk->pagz_mode_enabled = 0;

    // isEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_IS_ENABLED, idx);
    memset(rk->config_filename_is_enabled, 0, sizeof(rk->config_filename_is_enabled));
    strcpy(rk->config_filename_is_enabled, filename);
    int ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->enabled = ret;

    // isLeft
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_IS_LEFT, idx);
    memset(rk->config_filename_is_left, 0, sizeof(rk->config_filename_is_left));
    strcpy(rk->config_filename_is_left, filename);
    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->side = ret;

    rk->is_not_fault = rk_check_state;
    rk->azt_req_hndl = azt_req_handler;

    rk->stop = rk_stop;

    if(!rk->enabled) {
    	printf("INFO. %s RK disabled\r\n", rk->side == left ? "Left" : "Right");
    	return 0;
    }

    CAN_init(idx, &rk->can_bus);
#ifndef SIMULATION
    rk->can_bus.transmit(&rk->can_bus, FIRMWARE_VERSION, FIRMWARE_SUBVERSION, DEVICE_MARKING_CODE);
#else
    rk->can_bus.transmit(&rk->can_bus, 0.00, 0.00, 0.00);
#endif
    error_init(&rk->error_state);

    // address
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_ADDRESS, idx);
    memset(rk->config_filename_address, 0, sizeof(rk->config_filename_address));
    strcpy(rk->config_filename_address, filename);
    ret = parse_integer_config(filename);
    if((ret == -1) || (ret == 0)){
        return -1;
    }
    rk->address = ret;


    rk->process = rk_process;
    rk->state = trk_disabled_rk_installed;
    rk->state_issue = trk_state_issue_less_or_equal_dose;

    // isLocalControlEnabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_IS_PAGZ_MODE_ENABLED, idx);
    memset(rk->config_filename_is_local_control_enabled, 0, sizeof(rk->config_filename_is_local_control_enabled));
    strcpy(rk->config_filename_is_local_control_enabled, filename);
    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->pagz_mode_enabled = ret;

    // summator_price
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_SUMMATOR_PRICE, idx);
    memset(rk->config_filename_summator_price, 0, sizeof(rk->config_filename_summator_price));
    strcpy(rk->config_filename_summator_price, filename);
    float tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->summator_price = tmp;

    // gas_density
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_GAS_DENSITY, idx);
    memset(rk->config_filename_gas_density, 0, sizeof(rk->config_filename_gas_density));
    strcpy(rk->config_filename_gas_density, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->gas_density = tmp;

    // relay_cut_off_timing
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_RELAY_CUT_OFF_TIMING, idx);
    memset(rk->config_filename_relay_cut_off_timing, 0, sizeof(rk->config_filename_relay_cut_off_timing));
    strcpy(rk->config_filename_relay_cut_off_timing, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->relay_cut_off_timing = tmp;

    // mass_flow_rate_threshold_value
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_MASS_FLOW_RATE_THRESHOLD_VALUE, idx);
    memset(rk->config_filename_mass_flow_rate_threshold_value, 0, sizeof(rk->config_filename_mass_flow_rate_threshold_value));
    strcpy(rk->config_filename_mass_flow_rate_threshold_value, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->mass_flow_rate_threshold_value = tmp;

    // mass_flow_rate_break_error
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_MASS_FLOW_RATE_BREAK_ERROR, idx);
    memset(rk->config_filename_mass_flow_rate_break_error, 0, sizeof(rk->config_filename_mass_flow_rate_break_error));
    strcpy(rk->config_filename_mass_flow_rate_break_error, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->mass_flow_rate_break_error = tmp;

    // valves_amount
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_VALVES_AMOUNT, idx);
    memset(rk->config_filename_valves_amount, 0, sizeof(rk->config_filename_valves_amount));
    strcpy(rk->config_filename_valves_amount, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->valves_amount = tmp;

    // summator_volume
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_SUMMATOR_VOLUME, idx);
    memset(rk->config_filename_summator_volume, 0, sizeof(rk->config_filename_summator_volume));
    strcpy(rk->config_filename_summator_volume, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->summator_volume = tmp;

    // price_per_liter
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_PRICE_PER_LITER_MASK, idx);
    memset(rk->config_filename_price_per_liter, 0, sizeof(rk->config_filename_price_per_liter));
    strcpy(rk->config_filename_price_per_liter, filename);
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    rk->fueling_price_per_liter = tmp;

    // arm_level_software
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_ARM_LEVEL_SOFTWARE, idx);
    ret = parse_true_false_config(filename);
    if(ret == -1) {
        return -1;
    }
    rk->arm_level_sofrware = ret;
    printf("ERROR %s RK. arm level software: %d\r\n", rk->side == left ? "Left" : "Right", rk->arm_level_sofrware);

    ret = in_4_20_ma_init(idx, &rk->in_4_20, rk->enabled);

    rk->fueling_process_flag = 0;
    led_init(idx, &rk->led, &rk->error_state.code, &rk->fueling_process_flag);

    rk->modbus.side = rk->side;
    ret = gs_init_pthreaded(idx, &rk->modbus, rk->side);
    if(ret == -1) {
    	printf("ERROR %s RK. Modbus (flomac) initialization failed\r\n", rk->side == left ? "Left" : "Right");
    	return -1;
    }

    relay_init(idx, &rk->relay);

    counter_init(&rk->counter_stop_btn);
    counter_init(&rk->counter_reset_cmd);
    return 0;
}

static int rk_is_not_fault(rk_t* self) {
    return TRUE;
}

static int rk_check_state(rk_t* self) {
    if(!self->enabled) {
    	return 0;
    }
#ifndef DEV_WITHOUT_4_20
    // 4-20ma checkout
    int ret = in_4_20_ma_check_state(&self->in_4_20);

    if( (ret == IN_4_20_SWAPPED_WIRES_ERROR) && error_is_clear(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES) )
    {
      	printf("ERROR %s RK. 4-20ma swapped wires ERROR. state %d\r\n", self->side == left ? "Left" : "Right", ret);
        error_set(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES);
      	error_clear(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED);
        error_clear(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE);
        error_clear(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE);
        rk_indicate_error_message(self);
    }
    else if( (ret != IN_4_20_SWAPPED_WIRES_ERROR) && error_is_set(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES))
    {
      	printf("ERROR %s RK. 4-20ma swapped wires error restored. state %d\r\n", self->side == left ? "Left" : "Right", ret);
       	error_clear(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES);
       	if(ret == IN_4_20_NOT_CONNECTED_ERROR) {
       		// to be automatically cleaned by setting disconnection error
       	} else {
       		rk_indicate_error_message(self);
       	}
    }

    if( (ret == IN_4_20_NOT_CONNECTED_ERROR) && error_is_clear(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED) )
    {
    	printf("ERROR %s RK. 4-20ma disconnected ERROR. state %d\r\n", self->side == left ? "Left" : "Right", ret);
        error_set(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED);
    	error_clear(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES);
        error_clear(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE);
        error_clear(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE);
        rk_indicate_error_message(self);
    }
    else if( (ret != IN_4_20_NOT_CONNECTED_ERROR) && error_is_set(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED))
    {
    	printf("ERROR %s RK. 4-20ma connection restored. state %d\r\n", self->side == left ? "Left" : "Right", ret);
        error_clear(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED);
        rk_indicate_error_message(self);
    }
/*
    if( (ret == IN_4_20_LOW_PRESSURE_ERROR) && error_is_clear(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE) )
    {
    	printf("ERROR %s RK. 4-20ma low pressure ERROR. state %d\r\n", self->side == left ? "Left" : "Right", ret);
    	error_clear(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES);
        error_clear(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED);
        error_set(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE);
        error_clear(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE);
        rk_indicate_error_message(self);
    }
    else if( (ret != IN_4_20_LOW_PRESSURE_ERROR) && error_is_set(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE))
    {
    	printf("ERROR %s RK. 4-20ma low pressure error restored. state %d\r\n", self->side == left ? "Left" : "Right", ret);
        error_clear(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE);
        rk_indicate_error_message(self);
    }
*/
    if( (ret == IN_4_20_HIGH_PRESSURE_ERROR) && error_is_clear(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE) )
    {
    	printf("ERROR %s RK. 4-20ma high pressure ERROR. state %d\r\n", self->side == left ? "Left" : "Right", ret);
    	error_clear(&self->error_state, ERROR_INPUT_4_20_SWAPPED_WIRES);
        error_clear(&self->error_state, ERROR_INPUT_4_20_NOT_CONNECTED);
        error_clear(&self->error_state, ERROR_INPUT_4_20_LOW_PRESSURE);
        error_set(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE);
        rk_indicate_error_message(self);
    }
    else if( (ret != IN_4_20_HIGH_PRESSURE_ERROR) && error_is_set(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE))
    {
    	printf("ERROR %s RK. 4-20ma high pressure error restored. state %d\r\n", self->side == left ? "Left" : "Right", ret);
        error_clear(&self->error_state, ERROR_INPUT_4_20_HIGH_PRESSURE);
        rk_indicate_error_message(self);
    }

#endif

#ifndef DEV_WITHOUT_FLOMAC
    // modbus checkout
    ret = gs_check_state(&self->modbus);
    if( (ret == -1) && error_is_clear(&self->error_state, ERROR_MODBUS_NOT_CONNECTED) ) {
    	printf("MB ERROR. mb state %d\r\n", ret);
        error_set(&self->error_state, ERROR_MODBUS_NOT_CONNECTED);
        rk_indicate_error_message(self);
    } else if( (ret != -1) && error_is_set(&self->error_state, ERROR_MODBUS_NOT_CONNECTED)) {
    	printf("MB NOT ERROR. mb state %d\r\n", ret);
        error_clear(&self->error_state, ERROR_MODBUS_NOT_CONNECTED);
        rk_indicate_error_message(self);
    }

#endif

    // контроль обрыва шланга
    if( (self->flomac_mass_flowrate >= self->mass_flow_rate_break_error)
    		&& error_is_clear(&self->error_state, ERROR_HIGH_FLOW_MASS_RATE) ){
    	printf("FLOW RATE ERROR. %.2f\r\n", self->flomac_mass_flowrate);
        error_set(&self->error_state, ERROR_HIGH_FLOW_MASS_RATE);
        rk_indicate_error_message(self);
    } else if( (self->flomac_mass_flowrate < self->mass_flow_rate_break_error) && error_is_set(&self->error_state, ERROR_HIGH_FLOW_MASS_RATE)) {
    	printf("FLOW RATE restored %.2f\r\n", self->flomac_mass_flowrate);
        error_clear(&self->error_state, ERROR_HIGH_FLOW_MASS_RATE);
        rk_indicate_error_message(self);
    }

    return self->error_state.code == 0;
}

static void rk_indicate_error_message(rk_t* self) {
//    printf("%s RK indicate %f\r\n", self->side == left ? "Left" : "Right", self->error_state.message);
    self->can_bus.transmit(&self->can_bus, self->error_state.message, 0.00, 0.00);
}

static int rk_process(rk_t* self) {
    if(!self->enabled) {
    	return 0;
    }
    switch (self->state) {
        case trk_disabled_rk_installed:
        	self->fueling_process_flag = 0;
            break;
        case trk_disabled_rk_taken_off:
        	self->fueling_process_flag = 0;
            break;
        case trk_authorization_cmd:
        	self->fueling_process_flag = 1;
        	if(self->start_button_clicked_flag) {
        		if (self->pagz_mode_enabled) {
					self->start_button_delay_cnt++;
					if(self->stop_button_clicked_flag) {
						self->start_button_clicked_flag = 0;
						self->start_button_delay_cnt = 0;
						rk_stop_fueling_process(self, &self->cnt);
						self->stop_button_clicked_flag = 0;
						printf("%s RK. Fueling not started due to stop button being pressed: %d\r\n", self->side == left ? "Left" : "Right");
					}
					if(self->start_button_delay_cnt > DELAY_BETWEEN_PRESSING_START_BUTTON_AND_STARTING_FUELING) {
						self->start_button_clicked_flag = 0;
						self->start_button_delay_cnt = 0;
						printf("%s RK. Fueling start delay finished. Begin fueling: %d\r\n", self->side == left ? "Left" : "Right");
						button_start_handler(self);
					} else {
						if(self->start_button_delay_cnt % 10 == 0)
							printf("%s RK. Delay before starting fueling: %d\r\n", self->side == left ? "Left" : "Right",
									DELAY_BETWEEN_PRESSING_START_BUTTON_AND_STARTING_FUELING - self->start_button_delay_cnt);
					}
        		} else {  // в режиме ПАГЗ задержка после нажатия на кнопку СТАРТ не нужна
					self->start_button_clicked_flag = 0;
					self->start_button_delay_cnt = 0;
					printf("%s RK. Fueling started.\r\n", self->side == left ? "Left" : "Right");
					button_start_handler(self);
        		}
        	}
            break;
        case trk_enabled_fueling_process:
        	if(self->fueling_approved_by_human == 1) {
        		self->fueling_process_flag = 1;
            	rk_fueling_scheduler(self);
        	}
            break;
        case trk_enabled_fueling_process_local:
        	self->fueling_process_flag = 1;
            rk_fueling_scheduler(self);
            break;
        case trk_disabled_fueling_finished:
        	self->fueling_process_flag = 0;
            break;
        case trk_disabled_local_control_unit_dose:
            break;
        default:
            break;
    }
}

static int rk_stop(rk_t* self) {
    if(!self->enabled) {
    	return 0;
    }
	if( (self->state == trk_enabled_fueling_process) || (self->state == trk_enabled_fueling_process_local) ) {
		printf("%s RK. Alarm fueling interruption due to errors.\r\n", self->side == left ? "Left" : "Right");
    	rk_stop_fueling_process(self, &self->cnt);
        self->store_prev_summators_flag = 0;
        rk_fueling_log_results(self);
	}
}

static int rk_fueling_log(rk_t* self, int cnt, int necessary_flag) {
	float volume = self->fueling_current_volume + self->fueling_interrupted_volume;
	volume = round(volume * 100.0) / 100.0;
	float price = volume * self->fueling_price_per_liter;
	self->can_bus.transmit(&self->can_bus,
                           volume,
                           self->fueling_price_per_liter,
                           price);

	if((cnt % 100 == 0) || necessary_flag) {
		printf("%s RK. currently fueled %d: %.2f of %.2f dose (%.2f --> %.2f). rate: %.2f, pressure: %d (%.2f mA), summator: %.2f, interrupted: %.2f\r\n",
				self->side == left ? "Left" : "Right",
				cnt,
			   self->fueling_current_volume,
			   self->fueling_dose_in_liters,
			   self->flomac_inv_mass_starting_value,
			   self->flomac_inv_mass,
			   self->flomac_mass_flowrate,
			   self->pressure_4_20ma_raw,
			   self->pressure_4_20ma,
			   self->summator_volume,
			   self->fueling_interrupted_volume);
	}
	return 0;
}

static int rk_fueling_log_results(rk_t* self) {
    char volume_summator[12] = {0};
    sprintf(volume_summator, "%.2f", self->summator_volume);
    set_config(self->config_filename_summator_volume, volume_summator, strlen(volume_summator));
    self->fueling_interrupted_volume = 0.00;
    self->fueling_interrupted_price = 0.00;

    char price_summator[16] = {0};
    sprintf(price_summator, "%.2f", self->summator_price);
    set_config(self->config_filename_summator_price, price_summator, strlen(price_summator));
}

static int rk_fueling_calculate_summators(rk_t* self) {
    self->fueling_current_price = self->fueling_current_volume * self->fueling_price_per_liter;
    float prev_summator_volume_rounded = roundf(self->prev_summator_volume * 100) / 100;
    float fueling_current_volume_rounded = roundf(self->fueling_current_volume * 100) / 100;
    self->fueling_current_volume = fueling_current_volume_rounded;
    float fueling_interrupted_volume_rounded = roundf(self->fueling_interrupted_volume * 100) / 100;
    self->fueling_interrupted_volume = fueling_interrupted_volume_rounded;
    self->summator_volume = self->prev_summator_volume + self->fueling_current_volume + self->fueling_interrupted_volume;

    float prev_summator_price_rounded = roundf(self->prev_summator_price * 100) / 100;
    self->prev_summator_price = prev_summator_price_rounded;
    float fueling_current_price_rounded = roundf(self->fueling_current_price * 100) / 100;
    self->fueling_current_price = fueling_current_price_rounded;
    float fueling_interrupted_price_rounded = roundf(self->fueling_interrupted_price * 100) / 100;
    self->fueling_interrupted_price = fueling_interrupted_price_rounded;
    self->summator_price = self->prev_summator_price + self->fueling_current_price + self->fueling_interrupted_price;
}

static void rk_stop_fueling_process(rk_t* self, int* cnt) {
	valve_all_off(self);
    self->state = trk_disabled_fueling_finished;
    self->state_issue = trk_state_issue_less_or_equal_dose;
    self->fueling_approved_by_human = 0;  // сбрасываем флаг нажатия на кнопку "Старт"
    self->start_button_clicked_flag = 0;
    self->start_button_delay_cnt = 0;
    *cnt = 0;
}

static int rk_fueling_scheduler(rk_t* self) {
    self->cnt++;

    if(!self->store_prev_summators_flag) {
        self->prev_summator_volume = self->summator_volume;
        self->prev_summator_price = self->summator_price;
        self->store_prev_summators_flag = 1;
    }

    self->flomac_mass_flowrate = atomic_load(&self->modbus.mass_flowrate);

    if(self->fueling_dose_in_liters > 0.00) {

        if(self->fueling_current_volume < self->fueling_dose_in_liters) {
            self->flomac_inv_mass = atomic_load(&self->modbus.summator_mass);
//            self->flomac_mass_flowrate = atomic_load(&self->modbus.mass_flowrate);
            self->pressure_4_20ma_raw = atomic_load(&self->in_4_20.value);
            self->pressure_4_20ma = in_4_20_ma_convert_raw_to_ma(self->pressure_4_20ma_raw);
            self->fueling_current_volume = (self->flomac_inv_mass - self->flomac_inv_mass_starting_value) / self->gas_density;
            if(self->fueling_current_volume > self->fueling_dose_in_liters) {
                self->fueling_current_volume = self->fueling_dose_in_liters;
            }
        }

        // Процесс заправки может прекращаться в трех случаях:
        if(fabs(self->fueling_current_volume - self->fueling_dose_in_liters) <= self->relay_cut_off_timing)
        {
        	// 1. Заданная доза топлива заправлена
        	rk_stop_fueling_process(self, &self->cnt);
		self->fueling_current_volume = self->fueling_dose_in_liters;
        	rk_fueling_log(self, self->cnt, 1);
		self->store_prev_summators_flag = 0;
        	printf("%s RK. FUELING FINISHED #1. Requested dose fueled\r\n", self->side == left ? "Left" : "Right");
        }
        else if(self->reset_command_received_flag) {
        	// 4. Пришла команда "СТОП" с GasKit
        	if(!counter_is_started(&self->counter_reset_cmd)) {
            	printf("%s RK. FUELING FINISHED #4. Reset command received. Delay counter started.\r\n", self->side == left ? "Left" : "Right");
        		counter_start(&self->counter_reset_cmd);
        		valve_all_off(self);
        	} else {
        		if(counter_tick(&self->counter_reset_cmd)) {
        			counter_reset(&self->counter_reset_cmd);
                	self->reset_command_received_flag = 0;
                	rk_fueling_log_results(self);
                	if(self->arm_level_sofrware == arm_software_gas_kit) {
                		self->fueling_interrupted_volume = self->summator_volume - self->prev_summator_volume;
                		self->fueling_interrupted_price = self->summator_price - self->prev_summator_price;
                	}
                	rk_stop_fueling_process(self, &self->cnt);
                	printf("%s RK. FUELING FINISHED #4. Reset command received\r\n", self->side == left ? "Left" : "Right");
                	return;
        		} else {
        		}
        	}
        }
        else if((self->flomac_mass_flowrate < self->mass_flow_rate_threshold_value) && (self->cnt > 500) && (self->fueling_current_volume >= 0.05)) // 20 - для исключения вероятности, что mass flow rate возрастает не мгновенно после открытия клапана
        {
        	// 2. Бак заполнен (расход топлива снизился ниже порогового)
        	rk_fueling_log(self, self->cnt, 1);

        	if(self->valves_amount > SINGLE_VALVE) {
        		switch (self->current_valve_number) {
        		case FIRST_VALVE_FUELING_STAGE:
        			if(self->valves_amount > self->current_valve_number) {
        				valve_middle_on(self);
                		self->cnt = 0;
                		printf("%s RK. FUELING PROCESS. MIDDLE VALVE OPENED\r\n", self->side == left ? "Left" : "Right");
                		return;
        			}
        			break;
        		case SECOND_VALVE_FUELING_STAGE:
        			if(self->valves_amount > self->current_valve_number) {
        				valve_high_on(self);
                		self->cnt = 0;
                		printf("%s RK. FUELING PROCESS. HIGH VALVE OPENED\r\n", self->side == left ? "Left" : "Right");
                		return;
        			}
        			break;
        		case THIRD_VALVE_FUELING_STAGE:
            		printf("%s RK. FUELING PROCESS. LAST (HIGH) VALVE ALREADY OPENED\r\n", self->side == left ? "Left" : "Right");
        			break;
        		default:
        			printf("%s RK. ERROR. FUELING PROCESS. Unknown valve OPENED: %d\r\n", self->side == left ? "Left" : "Right", self->valves_amount);
        			break;
        		}
        	}

        	if(self->state == trk_enabled_fueling_process_local) {
        		// Если заправка была начата локально (с кнопки СТАРТ), то "забываем" объем заправленного топлива,
        		// чтобы в GasKit не возникла ошибка "Расхождения по счетчикам"
                rk_fueling_log(self, self->cnt, 1);
        		self->fueling_current_volume = 0.00;
        	} else {
                rk_fueling_log(self, self->cnt, 1);
        	}
        	rk_stop_fueling_process(self, &self->cnt);
            self->store_prev_summators_flag = 0;
        	printf("%s RK. FUELING FINISHED #2. FULL TANK Fueled due to low mass rate value: %.2f\r\n", self->side == left ? "Left" : "Right", self->flomac_mass_flowrate);
        }
        else if(self->stop_button_clicked_flag == 1) {
        	// 3. Нажата кнопка СТОП.
            	rk_fueling_log(self, self->cnt, 0);
            	if(!counter_is_started(&self->counter_stop_btn)) {
            		printf("%s RK. FUELING FINISHED #3. Stop button pressed. Delay counter started.\r\n", self->side == left ? "Left" : "Right");
            		counter_start(&self->counter_stop_btn);
            		valve_all_off(self);
            	} else {
            		if(counter_tick(&self->counter_stop_btn)) {
            			counter_reset(&self->counter_stop_btn);
            			rk_fueling_log(self, self->cnt, 1);
                		rk_stop_fueling_process(self, &self->cnt);
                		self->stop_button_clicked_flag = 0;
                		if(self->state == trk_enabled_fueling_process_local) {
                			self->fueling_current_volume = 0.00;
                		} else {
                			self->store_prev_summators_flag = 0;
                		}
                    	printf("%s RK. FUELING FINISHED #3. Stop button pressed\r\n", self->side == left ? "Left" : "Right");
            		} else {
            		}
            	}
        	}

        if(self->state != trk_enabled_fueling_process_local) {
        	// При локальной заправке (с кнопки СТАРТ) значения сумматоров не вычисляем, чтобы
        	// в GasKit не возникла ошибка "Расхождения по счетчикам"
        	rk_fueling_calculate_summators(self);
        }

        if(self->state != trk_disabled_fueling_finished)
        {
        	rk_fueling_log(self, self->cnt, 0);
        	if(self->fueling_current_volume <= 0.1) {
        		self->cnt = 0;
        	}
        }

        if(self->state == trk_disabled_fueling_finished) {
        	rk_fueling_log_results(self);
        }
    } else {
    }
}

static int azt_req_handler(azt_request_t* req, rk_t* self)
{
	static reset_current_fueling_values_flag = FALSE;

    if(!self->enabled) {
    	return 0;
    }

	if(req->address != self->address) {
        return 0;
    }

    char responce[AZT_RESPONCE_MAX_LENGTH] = {0};
    int cnt = 0;

    int higher_bits = 0;
    int lower_bits = 0;

    int tmp;
    int ret;

    char volume[7] = {0};
    char price_[8] = {0};
    char price[7] = {0};

    switch (req->cmd) {
        case AZT_REQUEST_TRK_STATUS_REQUEST:
		// printf("%s RK. Address %d. AZT_REQUEST_TRK_STATUS_REQUEST\n", self->side == left ? "Left" : "Right", self->address);

		if(self->arm_level_sofrware == arm_software_gas_kit) {

            		cnt = 0;
            		memset(responce, 0, sizeof(responce));

            		responce[cnt] = self->state;
            		if(self->state == trk_enabled_fueling_process_local) {
            			responce[cnt] = trk_disabled_rk_installed;
            		}
            		cnt++;
            		responce[cnt] = self->state_issue;
            		cnt++;
        	} else if(self->arm_level_sofrware == arm_software_doms) {

        		cnt = 0;
        		memset(responce, 0, sizeof(responce));

        		responce[cnt] = self->state;
        		if(self->state == trk_enabled_fueling_process_local) {
        			responce[cnt] = trk_disabled_rk_installed;
        		}
        		cnt++;
        		if(self->state == trk_disabled_fueling_finished) {
        			responce[cnt] = self->state_issue;
        			cnt++;
        		}
        	}

            azt_tx(responce, cnt);
            break;
        case AZT_REQUEST_TRK_AUTHORIZATION:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_AUTHORIZATION\n", self->side == left ? "Left" : "Right", self->address);
            ret = 0;
            if(self->is_not_fault(self)) {
                self->state = trk_authorization_cmd;
                azt_tx_ack();
		self->can_bus.transmit(&self->can_bus,
			self->fueling_dose_in_liters,
			0.00,
			self->fueling_dose_in_liters * self->fueling_price_per_liter);
            } else {
            	printf("%s RK. FUELING can't be started due to ERROR state: %08X\r\n", self->side == left ? "Left" : "Right", self->error_state.code);
            	azt_tx_can();
            	self->state = trk_disabled_rk_taken_off;
            }
            break;
        case AZT_REQUEST_TRK_RESET:
            printf("%s RK. Address %d. AZT_REQUEST_TRK_RESET\n", self->side == left ? "Left" : "Right", self->address);
            ret = 0;  // To-Do: implement checkout whether we can start discharging
            // Возможные статусы ТРК до запроса: ‘2’ ,‘3’ или ‘8’
            // Возможные статусы ТРК после запроса: ‘4’ + ‘0’  или  ‘4’+’1’
            //									    ‘1’ или ‘0’

            if(self->state == trk_enabled_fueling_process) { // если идет заправка
                self->reset_command_received_flag = 1;
                azt_tx_ack();
            } else {
            	if(ret == 0) {
					self->state = trk_disabled_fueling_finished;
					self->state_issue = trk_state_issue_less_or_equal_dose; // To-Do: implement correct issue setup
					azt_tx_ack();
				} else {
					azt_tx_can();
				}
            }
            break;
        case AZT_REQUEST_CURRENT_FUEL_CHARGE_VALUE:  // 0x34
            // printf("%s RK. Address %d. AZT_REQUEST_CURRENT_FUEL_CHARGE_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            if(self->arm_level_sofrware == arm_software_doms) {
            	char responce[AZT_RESPONCE_MAX_LENGTH] = {0};
            	memset(responce, 0, sizeof(responce));
            	char fueling_current_vol[VOLUME_DIGITS+1] = {0};
            	sprintf(fueling_current_vol, "%07.2f", fabs(self->fueling_current_volume));
            	fueling_current_vol[4] = fueling_current_vol[5];
            	fueling_current_vol[5] = fueling_current_vol[6];
            	fueling_current_vol[6] = 0x00;

            	strcpy(responce, fueling_current_vol);
				cnt = VOLUME_DIGITS;
//            	printf("%s RK. Address %d. responce 0x34:%f     %s\n", self->side == left ? "Left" : "Right", self->address, self->fueling_current_volume, responce);
				azt_tx(responce, cnt);
            }
            break;
        case AZT_REQUEST_FULL_FUELING_VALUE:  // 0x35
//            printf("%s RK. Address %d. AZT_REQUEST_FULL_FUELING_VALUE\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            self->fueling_current_price = self->fueling_current_volume * self->fueling_price_per_liter;

            if(self->arm_level_sofrware == arm_software_gas_kit) {
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
            } else if(self->arm_level_sofrware == arm_software_doms) {

            	char fueling_current_volume[5+1] = {0};
            	sprintf(fueling_current_volume, "%06.2f", self->fueling_current_volume);
            	fueling_current_volume[3] = fueling_current_volume[4];
            	fueling_current_volume[4] = fueling_current_volume[5];
            	fueling_current_volume[5] = 0x00;

            	char fueling_current_price[7+1] = {0};
            	sprintf(fueling_current_price, "%08.2f", self->fueling_current_price);
            	fueling_current_price[5] = fueling_current_price[6];
            	fueling_current_price[6] = fueling_current_price[7];
            	fueling_current_price[7] = 0x00;

            	char fueling_price_per_liter_str[4+1] = {0};
            	sprintf(fueling_price_per_liter_str, "%06.2f", self->fueling_price_per_liter);
            	fueling_price_per_liter_str[3] = fueling_price_per_liter_str[4];
            	fueling_price_per_liter_str[4] = fueling_price_per_liter_str[5];
            	fueling_price_per_liter_str[5] = 0x00;//'0';

            	strcpy(responce, fueling_current_volume);
            	strcpy(responce + 5, fueling_current_price);
            	strcpy(responce + 5 + 7, fueling_price_per_liter_str );
				cnt = 5 + 7 + 4;
				azt_tx(responce, cnt);
            }
            if(reset_current_fueling_values_flag == TRUE) {
                reset_current_fueling_values_flag = FALSE;
            }
            break;
        case AZT_REQUEST_SUMMATORS_VALUE:  // 0x36
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            if(self->arm_level_sofrware == arm_software_gas_kit) {
            tmp = (int)roundf(self->summator_volume * 100.0);
            int_to_string_azt(tmp, responce, &cnt, 10);
            tmp = (int)roundf(self->summator_price * 100.0);
            int_to_string_azt(tmp, responce, &cnt, 10);
            azt_tx(responce, cnt);
            } else if (self->arm_level_sofrware == arm_software_doms) {
				tmp = (int)roundf(self->summator_volume * 100.0);
				int_to_string_azt(tmp, responce, &cnt, 8);
				tmp = (int)roundf(self->summator_price * 100.0);
				int_to_string_azt(tmp, responce, &cnt, 8);
				azt_tx(responce, cnt);
            }
            printf("%s RK. Address %d. AZT_REQUEST_SUMMATORS_VALUE %.2f\r\n", self->side == left ? "Left" : "Right", self->address, self->summator_volume);
            break;
        case AZT_M2M_TELECOM_GET_GAS_DENSITY:
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            tmp = (int)roundf(self->gas_density * 1000.0);
            int_to_string_azt(tmp, responce, &cnt, 4);
            azt_tx(responce, cnt);
            printf("%s RK. Address %d. AZT_REQUEST_GAS_DENSITY %.3f\r\n", self->side == left ? "Left" : "Right", self->address, self->gas_density);
            break;
        case AZT_REQUEST_TRK_TYPE:
//            printf("%s RK. Address %d. AZT_REQUEST_TRK_TYPE\n", self->side == left ? "Left" : "Right", self->address);
            cnt = 0;
            memset(responce, 0, sizeof(responce));
            responce[cnt] = AZT_TRK_TYPE;
            cnt++;
            azt_tx(responce, cnt);
            break;
        case AZT_REQUEST_RECORD_CONFIRMATION:
//            printf("%s RK. Address %d. AZT_REQUEST_RECORD_CONFIRMATION\n", self->side == left ? "Left" : "Right", self->address);
            azt_tx_ack();
            self->state = trk_disabled_rk_installed;
//            self->state_issue = trk_state_issue_less_or_equal_dose;  // To-Do: implement state issue setup
//            azt_tx_can();  // To-Do: when it might responce as not ACK?
            reset_current_fueling_values_flag = TRUE;
//            printf("\treset_current_fueling_values_flag = %d\r\n", reset_current_fueling_values_flag);
            break;
        case AZT_REQUEST_PROTOCOL_VERSION:
//            printf("%s RK. Address %d. AZT_REQUEST_PROTOCOL_VERSION\n", self->side == left ? "Left" : "Right", self->address);

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
            memset(price, 0, sizeof(price));
            price[0] = req->params[0];
            price[1] = req->params[1];
            price[2] = '.';
            price[3] = req->params[2];
            price[4] = req->params[3];
            self->fueling_price_per_liter = strtof(price, NULL);
            printf("%s RK. Address %d. AZT_REQUEST_PRICE_PER_LITER_SETUP: %.2f\n", self->side == left ? "Left" : "Right", self->address, self->fueling_price_per_liter);
            if(self->arm_level_sofrware == arm_software_doms) {
		// check whether ItOil sets price before each fueling. If so - no need to write in flash.
            	tmp = set_config(self->config_filename_price_per_liter, price, strlen(price));
            } else {
		tmp = 0;
	    }
            if(tmp == 0) {
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
            break;

        case AZT_M2M_TELECOM_SET_GAS_DENSITY:
            printf("%s RK. Address %d. AZT_M2M_TELECOM_REQUEST_GAS_DENSITY\n", self->side == left ? "Left" : "Right", self->address);
            char density[6] = {0};
            density[0] = req->params[0];
            density[1] = '.';
            density[2] = req->params[1];
            density[3] = req->params[2];
            density[4] = req->params[3];
            float gas_density = strtof(density, NULL);
            self->gas_density = gas_density;
            tmp = set_config(self->config_filename_gas_density, density, strlen(density));
            if(tmp == 0) {
                azt_tx_ack();
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP:
            // printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            azt_tx_can();
            break;
        case AZT_REQUEST_FUELING_DOSE_IN_RUBLES:
            memset(price_, 0, sizeof(price_));
            price_[0] = req->params[0];
            price_[1] = req->params[1];
            price_[2] = req->params[2];
            price_[3] = req->params[3];
            price_[4] = '.';
            price_[5] = req->params[4];
            price_[6] = req->params[5];
            float fueling_dose_in_rubles = strtof(price_, NULL);
            self->fueling_dose_in_liters = fueling_dose_in_rubles / self->fueling_price_per_liter;
            printf("%s RK. Address %d. AZT_REQUEST_FUELING_DOSE_IN_RUBLES. %.2f rub --> %.2f volume\n", self->side == left ? "Left" : "Right", self->address, fueling_dose_in_rubles, self->fueling_dose_in_liters);
            self->fueling_dose_in_rubles = 0.00;
            ret = 0;  // To-Do: implement checkout whether we can start fueling process
            if(ret == 0) {
                azt_tx_ack();
                self->can_bus.transmit(&self->can_bus,
                		   self->fueling_dose_in_liters,
                           0.00,
						   self->fueling_dose_in_liters * self->fueling_price_per_liter);
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_FUELING_DOSE_IN_LITERS:
            memset(volume, 0, sizeof(volume));
//            char volume[7] = {0};

            volume[0] = req->params[0];
            volume[1] = req->params[1];
            volume[2] = req->params[2];
            volume[3] = '.';
            volume[4] = req->params[3];
            volume[5] = req->params[4];
            float fueling_dose_in_liters = strtof(volume, NULL);
            self->fueling_dose_in_liters = fueling_dose_in_liters;
            self->fueling_dose_in_rubles = 0.00; // when workstation sets fuel dose volume we reset fuel dose in rubles
            printf("%s RK. Address %d. AZT_REQUEST_FUELING_DOSE_IN_LITERS: %.2f\n", self->side == left ? "Left" : "Right", self->address, self->fueling_dose_in_liters);

//            tmp = set_config(self->config_filename_price_per_liter, price, strlen(price));
            ret = 0;  // To-Do: implement checkout whether we can start fueling process
            if(ret == 0) {
                azt_tx_ack();
                self->can_bus.transmit(&self->can_bus,
                		   self->fueling_dose_in_liters,
                           0.00,
						   self->fueling_dose_in_liters * self->fueling_price_per_liter);
            } else {
                azt_tx_can();
            }
            break;
        case AZT_REQUEST_UNCONDITIONAL_START:
            printf("%s RK. Address %d. AZT_REQUEST_UNCONDITIONAL_START\n", self->side == left ? "Left" : "Right", self->address);
            ret = 0;
//            Возможные статусы ТРК до запроса:     ‘2’  // trk_authorization_cmd
//            Возможные статусы ТРК после запроса:  ‘3’  // trk_enabled_fueling_process
            if(self->is_not_fault(self)) {
            	rk_start_fueling_process(self);
                azt_tx_ack();
            } else {
            	printf("%s RK. FUELING can't be started due to ERROR state: %08X\r\n", self->side == left ? "Left" : "Right", self->error_state.code);
                azt_tx_can();
                self->state = trk_disabled_rk_taken_off;
            }
            break;
        case AZT_REQUEST_TRK_ADDRESS_CHANGE:
            printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_TRK_ADDRESS_CHANGE\n", self->side == left ? "Left" : "Right", self->address);
            azt_tx_can();
            break;
        case AZT_REQUEST_COMMON_PARAMETERS_SETUP:
            printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_COMMON_PARAMETERS_SETUP\n", self->side == left ? "Left" : "Right", self->address);
            break;
        case AZT_REQUEST_CURRENT_TRANSACTION:
            printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_CURRENT_TRANSACTION\n", self->side == left ? "Left" : "Right", self->address);
            // To-Do: Implement it.
            break;
        case AZT_REQUEST_READ_PARAMS:
//            printf("%s RK. Address %d. AZT_REQUEST_READ_PARAMS\n", self->side == left ? "Left" : "Right", self->address);
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

                        char trk_control_mode = self->pagz_mode_enabled == TRUE ? AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_AND_LOCAL_CONTROL : AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_CONTROL_ONLY;

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
            printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_WRITE_PARAMS\n", self->side == left ? "Left" : "Right", self->address);
            azt_tx_can();
            break;
        case AZT_REQUEST_CURRENT_DOSE_READING:
            printf("ERROR. NOT IMPLEMENTED. %s RK. Address %d. AZT_REQUEST_CURRENT_DOSE_READING\n", self->side == left ? "Left" : "Right", self->address);
            azt_tx_can();
            break;
        default:
            break;
    }
}

static void rk_store_starting_values(rk_t* self)
{
    self->fueling_current_volume = 0.00;
    self->flomac_inv_mass_starting_value = atomic_load(&self->modbus.summator_mass);
    printf("flomac inventory mass starting value: %f. summator_volume: %.2f\r\n", self->flomac_inv_mass_starting_value, self->summator_volume);
    self->fueling_current_price = 0.00;
}

static void rk_start_fueling_process(rk_t* self)
{
	rk_store_starting_values(self);
    printf("%s RK. Waiting for human to approve fueling\n", self->side == left ? "Left" : "Right");
}

static void rk_start_local_fueling_process(rk_t* self)
{
    self->state = trk_enabled_fueling_process_local;
    self->fueling_current_volume = 0.00;
	self->fueling_dose_in_liters = 500.0;
    self->flomac_inv_mass_starting_value = atomic_load(&self->modbus.summator_mass);
    printf("flomac inventory mass starting value: %f\r\n", self->flomac_inv_mass_starting_value);
    self->fueling_current_price = 0.00;
    valve_low_on(self);
}

static void button_start_callback(rk_t* self, int code)
{
    printf("%s RK. start btn pressed clbk\r\n", self->side == left ? "Left" : "Right");

    if(self->enabled == 0) {
	    printf("%s RK disabled. Start button click ignored.\r\n", self->side == left ? "Left" : "Right");
	    return;
    }

    self->start_button_currently_pressed_flag = 1;

    if(self->selfcheck_mode) {
    	printf("%s RK. selfcheckmode. Light green led\r\n", self->side == left ? "Left" : "Right");
    	set_config(self->led.normal_led_filename, "1", 1);
    	self->can_bus.transmit(&self->can_bus, 888888.88f, 8888.88f, 888888.88f);
    	return;
    }

    if(self->state == trk_authorization_cmd) 
    {
    	self->start_button_clicked_flag = 1;
    } 
    else if (self->pagz_mode_enabled) 
    {
	printf("paggz mode %d \r\n", self->pagz_mode_enabled);
    	printf("starting fueling in PAGZ mode\r\n");
	self->stop_button_clicked_flag = 0;
	self->start_button_clicked_flag = 1;
	self->state = trk_authorization_cmd;
    } 
    else if( (self->state == trk_disabled_rk_installed) && 
		(self->stop_button_currently_pressed_flag == 1) ) 
    {
	printf("%s RK. both buttons pressed.\r\n", self->side == left ? "Left" : "Right");
    	self->can_bus.transmit(&self->can_bus, self->summator_volume, 0.00, self->summator_price);
    }
}

static void button_start_released_callback(rk_t* self, int code)
{
    printf("%s RK. start btn released clbk\r\n", self->side == left ? "Left" : "Right");
    if(self->enabled == 0) {
	    return;
    }
    self->start_button_currently_pressed_flag = 0;
}

static void button_start_handler(rk_t* self)
{
    if(self->pagz_mode_enabled) {
    	printf("%s RK. LOCAL FUELING started by pressing start button\r\n", self->side == left ? "Left" : "Right");

    	if(self->state == trk_authorization_cmd) {
    		rk_start_local_fueling_process(self);
    	} else {
    		printf("%s RK. LOCAL FUELING not possible in %d state.\r\n", self->side == left ? "Left" : "Right", self->state);
    	}
    } else {
    	if(self->state == trk_authorization_cmd) {
    		rk_store_starting_values(self);
        	printf("%s RK. FUELING approved by human\r\n", self->side == left ? "Left" : "Right");
        	self->state = trk_enabled_fueling_process;
        	self->fueling_approved_by_human = 1;
        	self->stop_button_clicked_flag = 0;  // нажатие на кнопку СТАРТ сбрасывает нажатие на кнопку СТОП
        	valve_low_on(self);
    	}
    }
}

static void button_stop_callback(rk_t* self, int code)
{
    printf("%s RK. stop btn pressed clbk\r\n", self->side == left ? "Left" : "Right");
    if(self->enabled == 0) {
	    printf("%s RK disabled. Stop button click ignored.\r\n", self->side == left ? "Left" : "Right");
	    return;
    }

    self->stop_button_currently_pressed_flag = 1;

    if(self->selfcheck_mode) {
    	printf("%s RK. selfcheckmode. Light red led\r\n", self->side == left ? "Left" : "Right");
    	set_config(self->led.error_led_filename, "1", 1);
    	self->can_bus.transmit(&self->can_bus, self->address, 4444.44f, 444444.44f);
    	return;
    }

    if (self->stop_button_clicked_flag == 0) {
    	self->stop_button_clicked_flag = 1;
    }
}

static void button_stop_released_callback(rk_t* self, int code)
{
    printf("%s RK. stop btn released clbk\r\n", self->side == left ? "Left" : "Right");
    if(self->enabled == 0) {
	    return;
    }
    self->stop_button_currently_pressed_flag = 0;
}

static void int_to_string_azt(int val, char* res, int* cnt, int len) {
    int div = 0;
    switch(len) {
		case 4: div = 1000; break;
		case 8: div = 10000000; break;
		case 10: div = 1000000000; break;
		default: div = 1000000000; break;
    }

    for(int i=0; i<len; i++) {
        res[*cnt] = val / div + ASCII_ZERO;
//        printf("%d / %d -> %c. cnt: %d\n", val, div, res[*cnt], *cnt);
        if(res[*cnt] > ASCII_ZERO) {
            val = val % div;
        }
        div /= 10;
        (*cnt)++;
    }
}

static void valve_low_on(rk_t* self)
{
	// нижняя линия включена всегда в процессе заправки
	relay_low_on(&self->relay);
	self->current_valve_number = FIRST_VALVE_FUELING_STAGE;
}

static void valve_middle_on(rk_t* self)
{
	// средняя линия включается следом за нижней, когда расход топлива снижается до порогового.
	// Нижняя линия при этом остается включенной
	relay_middle_on(&self->relay);
	self->current_valve_number = SECOND_VALVE_FUELING_STAGE;
}

static void valve_high_on(rk_t* self)
{
	// высокая линия включается ВМЕСТО средней, когда расход топлива снова снижается до порогового
	// Нижняя линия при этом остается включенной
	relay_middle_off(&self->relay);
	relay_high_on(&self->relay);
	self->current_valve_number = THIRD_VALVE_FUELING_STAGE;
}

static void valve_all_off(rk_t* self)
{
	relay_high_off(&self->relay);
	relay_middle_off(&self->relay);
	relay_low_off(&self->relay);
	self->current_valve_number = NO_FUELING_CURRENTLY;
}
