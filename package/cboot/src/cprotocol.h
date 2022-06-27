/*
 * cprotocol.h
 *
 *  Created on: Jun 25, 2022
 *      Author: compic
 */

#ifndef _CPROTOCOL_H_
#define _CPROTOCOL_H_

#include <stdint.h>
#include <linux/can.h>

void can_protocol_make_connect_cmd(uint32_t caddr, struct can_frame* p_frame);
uint32_t can_protocol(uint32_t caddr, struct can_frame* p_frame);
int32_t can_protocol_set_file(char* file_name);
void can_protocol_close_file(void);

#endif /* _PROTOCOL_H_ */
