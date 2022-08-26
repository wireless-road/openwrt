//
// Created by al on 23.07.22.
//

#ifndef SRC_RK_H
#define SRC_RK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "libazt.h"
#include "settings.h"
#include "can.h"
#include "in_4_20_ma.h"
#include "error.h"
#include "libgs.h"
#include "led.h"

typedef enum { right, left } rk_side;

typedef enum {
    trk_disabled_rk_installed = 0x30,
    trk_disabled_rk_taken_off = 0x31,
    trk_authorization_cmd = 0x32,
    trk_enabled_fueling_process = 0x33,
    trk_disabled_fueling_finished = 0x34,
    trk_disabled_local_control_unit_dose = 0x38
} trk_state_t;

typedef enum {
    trk_state_issue_less_or_equal_dose = 0x30,
    trk_state_issue_overdose = 0x31
} trk_state_issue_t;

#define SIMULATION_FUELING_FULL_TANK_VOLUME	1.2f
#define CONFIG_FILENAME_MAX_LENGTH      64

#define PRICE_PER_LITER_DIGITS      6
#define VOLUME_DIGITS               6
#define PRICE_DIGITS                8

typedef struct rk_t rk_t;
struct rk_t {
    int enabled;
    rk_side side;
    trk_state_t state;
    int fueling_process_flag;
    trk_state_issue_t state_issue;
    int address;
    int local_control_allowed;
    float summator_volume;
    float summator_price;
    float prev_summator_volume;
    float prev_summator_price;
    int (*azt_req_hndl)(azt_request_t* req, rk_t* self);
    int (*is_not_fault)(rk_t* self);
    int (*process)(rk_t* self);
    char config_filename_is_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_left[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_address[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_local_control_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_price[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_volume[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_price_per_liter[CONFIG_FILENAME_MAX_LENGTH];
    float fueling_dose_in_liters;  // Заданная с АРМ доза к заправке в литрах
    float fueling_dose_in_rubles;  // Заданная с АРМ доза к заправке в рублях
    float fueling_price_per_liter;  // Заданная с АРМ стоимость топлива за литр
    float fueling_interrupted_volume;
    float fueling_interrupted_price;
    float fueling_current_volume;   // Текущий объем заправленного топлива в текущем цикле заправки
    float fueling_current_price;    // Текущая стоимость заправленного топлива в текущем цикле заправки
    int fueling_current_finished_flag;
    can_t can_bus;
    in_4_20_t in_4_20;
    error_t error_state;
    gs_conninfo_t modbus;
    void (*btn_clbk_start)(int);
    void (*btn_clbk_stop)(int);
    led_t led;
};

#define DEF_CONFIG_FILE_IS_ENABLED                  "/etc/gs/%d/isEnabled"
#define DEF_CONFIG_FILE_IS_LEFT                     "/etc/gs/%d/isLeft"
#define DEF_CONFIG_FILE_ADDRESS                     "/etc/gs/%d/address"
#define DEF_CONFIG_FILE_IS_LOCAL_CONTROL_ENABLED    "/etc/gs/%d/isLocalControlEnabled"
#define DEF_CONFIG_FILE_SUMMATOR_PRICE              "/etc/gs/%d/summatorPrice"
#define DEF_CONFIG_FILE_SUMMATOR_VOLUME             "/etc/gs/%d/summatorVolume"
#define DEF_CONFIG_FILE_PRICE_PER_LITER_MASK        "/etc/gs/%d/pricePerLiter"


#define CONFIG_FILE_IS_ENABLED                  "/mnt/gs/%d/isEnabled"
#define CONFIG_FILE_IS_LEFT                     "/mnt/gs/%d/isLeft"
#define CONFIG_FILE_ADDRESS                     "/mnt/gs/%d/address"
#define CONFIG_FILE_IS_LOCAL_CONTROL_ENABLED    "/mnt/gs/%d/isLocalControlEnabled"
#define CONFIG_FILE_SUMMATOR_PRICE              "/mnt/gs/%d/summatorPrice"
#define CONFIG_FILE_SUMMATOR_VOLUME             "/mnt/gs/%d/summatorVolume"
#define CONFIG_FILE_PRICE_PER_LITER_MASK        "/mnt/gs/%d/pricePerLiter"

int rk_init(int idx, rk_t* rk);

#endif //SRC_RK_H
