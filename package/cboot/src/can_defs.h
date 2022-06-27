/*
 * can_defs.h
 *
 *  Created on: Jun 23, 2022
 *      Author: compic
 */

#ifndef _CAN_DEFS_H_
#define _CAN_DEFS_H_

#include <stdint.h>

// 11 standart id
//  id bit number        10 9 8 7 6 5    4 3 2 1 0
//  id bit structure   |   dev addr   |  cmd code  |


#define CAN_ADDR_BIT_POSITION		(5U)
#define CAN_ADDR_MASK				(0x3FU << CAN_ADDR_BIT_POSITION)
#define CAN_ADDR					(1U << CAN_ADDR_BIT_POSITION)
#define CAN_ADD_MAX					(0x3FU)

#define CAN_MASTER_ADDR				(0U << CAN_ADDR_BIT_POSITION)

#define CAN_CMD_MASK				(0x1FU)

#define CMD_SET_HI_DATA				(0U)
#define CMD_SET_LO_DATA				(1U)
#define CMD_FW_UPDATE				(2U)
#define CMD_SET_ADDR				(3U)
#define CMD_ERASE					(4U)
#define CMD_WRITE					(5U)
#define CMD_READ					(6U)
#define CMD_GO_APP					(7U)
#define CMD_GET_STATE				(8U)

#define	STATE_APP					(1U)
#define	STATE_BOOTLOADER			(2U)

#define ERR_OK			(0U)
#define ERR_ERR			(0xFFU)

#define RESTART_DELAY	(40U)
#define	BOOT_WR_LEN		(2U)
#define BOOT_RD_LEN		(2U)

#define GO_APP_TIMEOUT	(40U)
#define RESTART_TIMEOUT	(40U)

#define RW_SIZE			(8U)

#define	START_APP_ADDR	(0x08004000U)
#define	END_APP_ADDR	(0x08010000U)
#define MCU_MEM_SZ		(END_APP_ADDR - START_APP_ADDR)
#define	ERASE_INC_ADDR	(1024U)
#define	WRITE_INC_ADDR	(8U)
#define	LEN_INFO_FILE_POSITION		(0x108U)

typedef struct {
	uint8_t status;
	uint32_t build_date;
	uint16_t build_number;
}__attribute__((packed)) t_status_answer;

typedef struct {
	uint32_t address;
}__attribute__((packed)) t_set_address_cmd;

typedef struct {
	uint8_t _err;
}__attribute__((packed)) t_std_answer;

typedef struct {
	uint32_t data[2];
}__attribute__((packed)) t_write_cmd;

typedef struct {
	uint32_t data[2];
}__attribute__((packed)) t_read_answer;

typedef struct{
	uint8_t data[8];
	uint32_t len;
	uint32_t id;
}t_rcv_data;


#endif /* _CAN_DEFS_H_ */
