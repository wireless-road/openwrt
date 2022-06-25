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


#define CAN_ADDR_BIT_POSITION		(5UL)
#define CAN_ADDR_MASK				(0x3FUL << CAN_ADDR_BIT_POSITION)
#define CAN_ADDR					(1UL << CAN_ADDR_BIT_POSITION)
#define CAN_ADD_MAX					(0x3FUL)

#define CAN_MASTER_ADDR				(0UL << CAN_ADDR_BIT_POSITION)

#define CAN_CMD_MASK				(0x1FUL)

#define CMD_SET_HI_DATA				(0UL)
#define CMD_SET_LO_DATA				(1UL)
#define CMD_FW_UPDATE				(2UL)
#define CMD_SET_ADDR				(3UL)
#define CMD_ERASE					(4UL)
#define CMD_WRITE					(5UL)
#define CMD_READ					(6UL)
#define CMD_GO_APP					(7UL)
#define CMD_GET_STATE				(8UL)

#define	STATE_APP					(1UL)
#define	STATE_BOOTLOADER			(2UL)

#define ERR_OK			(0UL)
#define ERR_ERR			(0xFFUL)

#define RESTART_DELAY	(40UL)
#define	BOOT_WR_LEN		(2UL)
#define BOOT_RD_LEN		(2UL)

#define GO_APP_TIMEOUT	(40UL)
#define RESTART_TIMEOUT	(40UL)

#define	START_APP_ADDR	(0x08004000UL)
#define	END_APP_ADDR	(0x08010000UL)
#define MCU_MEM_SZ		(END_APP_ADDR - START_APP_ADDR)
#define	ERASE_INC_ADDR	(0UL)
#define	WRITE_INC_ADDR	(8UL)
#define	LEN_INFO_FILE_POSITION		(0x108)

typedef enum{
	ST_BOOT_CONNECT = 0,
	ST_BOOT_TO_BOOT,
	ST_BOOT_SET_ERASE_ADDR,
	ST_BOOT_ERASE,
	ST_BOOT_SET_WRITE_ADDR,
	ST_BOOT_WRITE,
	ST_BOOT_SET_READ_ADDR,
	ST_BOOT_READ,
	ST_BOOT_TO_APP,
	ST_BOOT_GET_STATE_AFTER_UPDATE,
}t_boot_state;

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
