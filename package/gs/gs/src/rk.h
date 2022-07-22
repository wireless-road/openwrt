//
// Created by al on 23.07.22.
//

#ifndef SRC_RK_H
#define SRC_RK_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

typedef enum { right, left } rk_side;

#define DIGIT_CHAR_TO_NUM(a)(a -= 0x30)

#define TRUE    1
#define FALSE   0

typedef struct {
    int enabled;
    rk_side side;
    int address;
} rk_t;

int rk_init(int idx, rk_t* rk);

#endif //SRC_RK_H
