//
// Created by al on 23.07.22.
//

#ifndef SRC_RK_H
#define SRC_RK_H

#include <stdio.h>
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

typedef struct rk_t rk_t;
struct rk_t {
    int enabled;
    rk_side side;
    trk_state_t state;
    trk_state_issue_t state_issue;
    int address;
    int local_control_allowed;
    int (*azt_req_hndl)(azt_request_t* req, rk_t* self);
};

int rk_init(int idx, rk_t* rk);

#endif //SRC_RK_H
