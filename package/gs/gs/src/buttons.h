#ifndef BUTTONS_H
#define BUTTONS_H

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/input.h>

#define INPUT_DEV "/dev/input/event0"

int buttons_init(void);
int buttons_handler(int *fd);
extern void button_handler(int code);

#endif
