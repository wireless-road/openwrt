/*
 * cprotocol.c
 *
 *  Created on: Jun 25, 2022
 *      Author: compic
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "can_defs.h"
#include "cprotocol.h"

t_boot_state proto_state;
uint32_t addr;
uint32_t sent_size;

uint32_t can_protocol(FILE* fr, uint32_t caddr, struct can_frame* p_frame)
{
	uint32_t ret_val = 0;
	uint8_t rcv_data[8];

	switch(proto_state)
	{
		case ST_BOOT_CONNECT:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_GET_STATE)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_status_answer* p_answ = (t_status_answer*)rcv_data;
					if(p_answ->status == STATE_BOOTLOADER)
					{
						p_frame->can_id = caddr + CMD_SET_ADDR;
						addr = START_APP_ADDR;
						memcpy(p_frame->data, &addr, sizeof(addr));
						p_frame->can_dlc = sizeof(addr);

						proto_state = ST_BOOT_SET_ERASE_ADDR;

						ret_val = 1;
					}else if(p_answ->status == STATE_APP)
					{
						p_frame->can_id = caddr + CMD_FW_UPDATE;
						p_frame->can_dlc = 0;

						proto_state = ST_BOOT_TO_BOOT;

						ret_val = 1;
					}
				}
			}
			break;
		case ST_BOOT_TO_BOOT:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_FW_UPDATE)
				{
					sleep(1);
					p_frame->can_id = caddr + CMD_GET_STATE;
					p_frame->can_dlc = 0;

					proto_state = ST_BOOT_CONNECT;

					ret_val = 1;
				}
			}
			break;
		case ST_BOOT_SET_ERASE_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						addr = START_APP_ADDR;
						p_frame->can_id = caddr + CMD_ERASE;
						p_frame->can_dlc = 0;
						printf("now erase\n");
						proto_state = ST_BOOT_ERASE;
						ret_val = 1;
					}
				}
			}
			break;
		case ST_BOOT_ERASE:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_ERASE)
				{
					if(addr < END_APP_ADDR)
					{
						memcpy(rcv_data, p_frame->data, 8);
						t_std_answer* p_answ = (t_std_answer*)rcv_data;
						if(p_answ->_err == ERR_OK)
						{
							addr += ERASE_INC_ADDR;
							p_frame->can_id = caddr + CMD_ERASE;
							p_frame->can_dlc = 0;
							ret_val = 1;
						}else{
							printf("error while erasing 1\n");
							ret_val = 2;
						}
					}else{
						addr = START_APP_ADDR;
						p_frame->can_id = caddr + CMD_SET_ADDR;
						addr = START_APP_ADDR;
						memcpy(p_frame->data, &addr, sizeof(addr));
						p_frame->can_dlc = sizeof(addr);
						proto_state = ST_BOOT_SET_WRITE_ADDR;
						ret_val = 1;
					}
				}
			}
			break;
		case ST_BOOT_SET_WRITE_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						size_t bytes_read = fread(rcv_data, 8, 1, fr);

						p_frame->can_id = caddr + CMD_WRITE;
						memcpy(p_frame->data, rcv_data, bytes_read);
						p_frame->can_dlc = bytes_read;
						addr += bytes_read;
						proto_state = ST_BOOT_WRITE;
						ret_val = 1;

					}else{
						printf("error while erasing 2\n");
						ret_val = 2;
					}
				}
			}
			break;
		case ST_BOOT_WRITE:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_WRITE)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						if(addr < END_APP_ADDR)
						{
							size_t bytes_read = fread(rcv_data, 8, 1, fr);

							p_frame->can_id = caddr + CMD_WRITE;
							memcpy(p_frame->data, rcv_data, bytes_read);
							p_frame->can_dlc = bytes_read;
							addr += bytes_read;
							proto_state = ST_BOOT_WRITE;
							ret_val = 1;
						}else{
							uint32_t res = fseek(fr, 0, SEEK_SET);
							if(res)
							{
								printf("can not seek position 1\n");
								ret_val = 2;
							}else{
								addr = START_APP_ADDR;
								p_frame->can_id = caddr + CMD_SET_ADDR;
								memcpy(p_frame->data, &addr, sizeof(addr));
								p_frame->can_dlc = sizeof(addr);
								proto_state = ST_BOOT_SET_READ_ADDR;
								ret_val = 1;
							}
						}
					}else{
						printf("error while writing 1\n");
						ret_val = 2;
					}
				}
			}
			break;
		case ST_BOOT_SET_READ_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						p_frame->can_id = caddr + CMD_READ;
						p_frame->can_dlc = 0;
						proto_state = ST_BOOT_READ;
						ret_val = 1;
					}else{
						printf("error while writing 2\n");
						ret_val = 2;
					}
				}
			}
			break;
		case ST_BOOT_READ:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_READ)
				{
					memcpy(rcv_data, p_frame->data, p_frame->can_dlc);
					uint8_t rd_data[8];
					size_t bytes_read = fread(rd_data, 8, 1, fr);
					int32_t res = memcmp(rcv_data, rd_data, bytes_read);
					if(res == 0)
					{
						addr += p_frame->can_dlc;
						if(addr < MCU_MEM_SZ)
						{
							p_frame->can_id = caddr + CMD_READ;
							p_frame->can_dlc = 0;
							ret_val = 1;
						}else{
							printf("verify OK\n");
							printf("now go to APP\n");

							proto_state = ST_BOOT_TO_APP;

							p_frame->can_id = caddr + CMD_GO_APP;
							p_frame->can_dlc = 0;
							ret_val = 1;
						}
					}else{
						printf("verify error at adddress - 0x%08X\n", addr);
						ret_val = 2;
					}
				}
			}
			break;
		case ST_BOOT_TO_APP:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_GO_APP)
				{
					sleep(1);
					p_frame->can_id = caddr + CMD_GET_STATE;
					p_frame->can_dlc = 0;

					proto_state = ST_BOOT_GET_STATE_AFTER_UPDATE;

					ret_val = 1;
				}
			}
			break;
		case ST_BOOT_GET_STATE_AFTER_UPDATE:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_GET_STATE)
				{
					memcpy(rcv_data, p_frame->data, 8);
					t_status_answer* p_answ = (t_status_answer*)rcv_data;
					if(p_answ->status == STATE_APP)
					{
						printf("update successful\n");

						ret_val = 3;
					}
				}
			}
			break;
	}

	return ret_val;
}

//------------------- End of File ------------------
