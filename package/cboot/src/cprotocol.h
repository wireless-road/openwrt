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

uint32_t can_protocol(FILE* fr, uint32_t caddr, struct can_frame* p_frame);

#endif /* _PROTOCOL_H_ */
