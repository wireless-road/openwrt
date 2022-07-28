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

typedef enum { right, left } rk_side;

typedef enum {
    trk_disabled_rk_installed = 0x30,
    trk_disabled_rk_taken_off = 0x31,
    trk_authorization_cmd = 0x32,
    trk_enabled_fuel_dischargning_process = 0x33,
    trk_disabled_fuel_discharging_finished = 0x34,
    trk_disabled_local_control_unit_dose = 0x38
} trk_state_t;

typedef enum {
    trk_state_issue_less_or_equal_dose = 0x30,
    trk_state_issue_overdose = 0x31
} trk_state_issue_t;

#define DIGIT_CHAR_TO_NUM(a)(a -= 0x30)

#define TRUE    1
#define FALSE   0

#define CONFIG_FILENAME_MAX_LENGTH      64

typedef struct rk_t rk_t;
struct rk_t {
    int enabled;
    rk_side side;
    trk_state_t state;
    trk_state_issue_t state_issue;
    int address;
    int local_control_allowed;
    float summator_volume;
    float summator_price;
    float price_per_liter;
    int (*azt_req_hndl)(azt_request_t* req, rk_t* self);
    char config_filename_is_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_left[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_address[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_local_control_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_price[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_volume[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_price_per_liter[CONFIG_FILENAME_MAX_LENGTH];
};

#define DEF_CONFIG_FILE_IS_ENABLED                  "/etc/gs/%d/isEnabled"
#define DEF_CONFIG_FILE_IS_LEFT                     "/etc/gs/%d/isLeft"
#define DEF_CONFIG_FILE_ADDRESS                     "/etc/gs/%d/address"
#define DEF_CONFIG_FILE_IS_LOCAL_CONTROL_ENABLED    "/etc/gs/%d/isLocalControlEnabled"
#define DEF_CONFIG_FILE_SUMMATOR_PRICE              "/etc/gs/%d/summatorPrice"
#define DEF_CONFIG_FILE_SUMMATOR_VOLUME             "/etc/gs/%d/summatorVolume"
#define DEF_CONFIG_FILE_PRICE_PER_LITER_MASK        "/etc/gs/%d/pricePerLiter"

// To-Do:
// implement usage of microSD based files to:
// 1. keep SPI NOR flash writing/erasing resource
// 2. prevent device bricking by supply power loosing on writing to SPI NOR
// E.g. on start application should check whether following (listed below) microSD files exists.
// If no it should created them by copying default files (listed above) from SPI NOR to microSD.
// Do not copy file in reverse from microSD to SPI NOR never!

#define CONFIG_FILE_IS_ENABLED                  "/mnt/gs/%d/isEnabled"
#define CONFIG_FILE_IS_LEFT                     "/mnt/gs/%d/isLeft"
#define CONFIG_FILE_ADDRESS                     "/mnt/gs/%d/address"
#define CONFIG_FILE_IS_LOCAL_CONTROL_ENABLED    "/mnt/gs/%d/isLocalControlEnabled"
#define CONFIG_FILE_SUMMATOR_PRICE              "/mnt/gs/%d/summatorPrice"
#define CONFIG_FILE_SUMMATOR_VOLUME             "/mnt/gs/%d/summatorVolume"
#define CONFIG_FILE_PRICE_PER_LITER_MASK        "/mnt/gs/%d/pricePerLiter"

int rk_init(int idx, rk_t* rk);

#endif //SRC_RK_H
