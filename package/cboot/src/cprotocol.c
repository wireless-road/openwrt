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
#include "stm32_crc32.h"

typedef enum{
	ST_BOOT_CONNECT = 0,
	ST_BOOT_TO_BOOT,
	ST_BOOT_SET_ERASE_ADDR,
	ST_BOOT_ERASE,
	ST_BOOT_SET_WRITE_ADDR,
	ST_BOOT_WRITE,
	ST_BOOT_WRITE_CRC32,
	ST_BOOT_SET_READ_ADDR,
	ST_BOOT_READ,
	ST_BOOT_READ_CRC32,
	ST_BOOT_TO_APP,
	ST_BOOT_GET_STATE_AFTER_UPDATE,
}t_boot_state;

t_boot_state proto_state = ST_BOOT_CONNECT;
uint32_t addr;
uint32_t sent_size;
static FILE* f_read;
static uint32_t f_len;
static uint32_t crc32;
uint32_t rd_crc32;
uint32_t fw_build_date;
uint32_t fw_build_number;
uint32_t cmd_snd_cnt;

void can_protocol_set_file_len(uint32_t len)
{
	f_len = len;
}

int32_t can_protocol_set_file(char* file_name)
{
	long int file_len;
	uint32_t file_name_len = strlen(file_name);
	if(file_name_len == 0)
	{
		printf("file name not set\n");
		return -1;
	}

	f_read = fopen(file_name, "rb");
	if(f_read == NULL)
	{
		printf("can not open file\n");
		return -1;
	}

	int res = fseek(f_read, 0, SEEK_END);
	if(res)
	{
		fclose(f_read);
		printf("can not seek position 0\n");
		return -1;
	}

	file_len = ftell(f_read);
	if(file_len <= 0L)
	{
		fclose(f_read);
		printf("get file size error\n");
		return -1;
	}

	printf("file size - %ld\n", file_len);

	res = fseek(f_read, LEN_INFO_FILE_POSITION, SEEK_SET);
	if(res)
	{
		fclose(f_read);
		printf("can not seek position 1\n");
		return -1;
	}

	uint32_t mem_len;
	size_t bytes_read = fread(&mem_len, sizeof(mem_len), 1, f_read);
	if(bytes_read != 1)
	{
		printf("bytes_read - %u\n", bytes_read);
		printf("can not read file\n");
		fclose(f_read);
		return -1;
	}

	mem_len *= 4U;
	printf("mem_len - %u\n", mem_len);

	if(mem_len > MCU_MEM_SZ)
	{
		fclose(f_read);
		printf("file length is big\n");
		return -1;
	}

	if(mem_len != (uint32_t)file_len)
	{
		fclose(f_read);
		printf("file size is big\n");
		return -1;
	}

	file_len -= 4;

	f_len = (uint32_t)file_len;

	// read build data and build vercion
	res = fseek(f_read, BUILD_DATE_POS, SEEK_SET);
	if(res)
	{
		fclose(f_read);
		printf("can not seek position 1.1\n");
		return -1;
	}

	bytes_read = fread(&fw_build_date, sizeof(fw_build_date), 1, f_read);
	if(bytes_read != 1)
	{
		printf("bytes_read - %u\n", bytes_read);
		printf("can not read build date\n");
		fclose(f_read);
		return -1;
	}

	res = fseek(f_read, BUILD_NUMBER_POS, SEEK_SET);
	if(res)
	{
		fclose(f_read);
		printf("can not seek position 1.1\n");
		return -1;
	}

	bytes_read = fread(&fw_build_number, sizeof(fw_build_number), 1, f_read);
	if(bytes_read != 1)
	{
		printf("bytes_read - %u\n", bytes_read);
		printf("can not read build number\n");
		fclose(f_read);
		return -1;
	}

	fw_build_number &= 0xFFFFUL;  // to uint16_t value

	//---------------------------------

	res = fseek (f_read, 0, SEEK_SET);
	if(res)
	{
		fclose(f_read);
		printf("can not seek position 2\n");
		return -1;
	}

	return 0;
}

void can_protocol_make_connect_cmd(uint32_t caddr, struct can_frame* p_frame)
{
	cmd_snd_cnt = 0;
	p_frame->can_id = caddr + CMD_GET_STATE;
	p_frame->can_dlc = 0;
}

uint32_t can_protocol(uint32_t caddr, struct can_frame* p_frame)
{
	uint32_t ret_val = 0;
	uint8_t rcv_data[RW_SIZE];

	switch(proto_state)
	{
		case ST_BOOT_CONNECT:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_GET_STATE)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_status_answer* p_answ = (t_status_answer*)rcv_data;
					printf("Dev state\n");
					if(p_answ->status == STATE_BOOTLOADER)
					{
						printf("     BOOTLOADER state\n");
						if(p_answ->build_date == 0xFFFFFFFFUL)
						{
							printf("     device build data   --------\n");
						}else{
							printf("     device build data   %u\n", p_answ->build_date);
						}
						if(p_answ->build_number == 0xFFFFUL)
						{
							printf("     device build number ----\n");
						}else{
							printf("     device build number %u\n", p_answ->build_number);
						}
						printf("     fw build data       %u\n", fw_build_date);
						printf("     fw build number     %u\n", fw_build_number);

						char resp = 0;
						do{
							printf("do you want continue ? (y/n): ");
							fflush(stdout);
							scanf("%c", &resp);
							if((resp == 'n') || (resp == 'N'))
							{
								printf("update process aborted\n");
								if(p_answ->build_date == 0xFFFFFFFFUL)
								{
									ret_val = 3U;
								}else{
									proto_state = ST_BOOT_TO_APP;
									p_frame->can_id = caddr + CMD_GO_APP;
									p_frame->can_dlc = 0;
									ret_val = 1U;
								}
							}
							if((resp == 'y') || (resp == 'Y'))
							{
								printf("Send set erase address cmd\n");
								p_frame->can_id = caddr + CMD_SET_ADDR;
								addr = START_APP_ADDR;
								memcpy(p_frame->data, &addr, sizeof(addr));
								p_frame->can_dlc = sizeof(addr);

								proto_state = ST_BOOT_SET_ERASE_ADDR;

								ret_val = 1U;
							}
							fflush(stdin);
						}while((resp != 'n') && (resp != 'y') && (resp != 'N') && (resp != 'Y'));
					}else if(p_answ->status == STATE_APP)
					{
						printf("     APPLICATION state\n");
						printf("Send go to bootloader cmd\n");
						p_frame->can_id = caddr + CMD_FW_UPDATE;
						p_frame->can_dlc = 0;

						proto_state = ST_BOOT_TO_BOOT;

						ret_val = 1U;
					}
				}
			}
			break;
		case ST_BOOT_TO_BOOT:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_FW_UPDATE)
				{
					printf("Send go to bootloader answer\n");
					sleep(1);
					printf("Check state\n");
					p_frame->can_id = caddr + CMD_GET_STATE;
					p_frame->can_dlc = 0;

					proto_state = ST_BOOT_CONNECT;

					ret_val = 1U;
				}
			}
			break;
		case ST_BOOT_SET_ERASE_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_set_address_answer* p_answ = (t_set_address_answer*)rcv_data;
					printf("set erase address cmd answer\n");
					if(p_answ->_err == ERR_OK)
					{
						printf("   status OK\n");
						printf("   addr 0x%08X OK\n", p_answ->addr);
						addr = START_APP_ADDR + ERASE_INC_ADDR;
						p_frame->can_id = caddr + CMD_ERASE;
						p_frame->can_dlc = 0;
						printf("now erase\n");
						proto_state = ST_BOOT_ERASE;
						ret_val = 1U;
					}
				}
			}
			break;
		case ST_BOOT_ERASE:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_ERASE)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_erase_answer* p_answ = (t_erase_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						if(addr < END_APP_ADDR)
						{
							fprintf(stderr, ".");
							addr += ERASE_INC_ADDR;
							p_frame->can_id = caddr + CMD_ERASE;
							p_frame->can_dlc = 0;
							ret_val = 1U;
						}else{
							printf("\n");
							printf("erase OK\n");
							p_frame->can_id = caddr + CMD_SET_ADDR;
							addr = START_APP_ADDR;
							memcpy(p_frame->data, &addr, sizeof(addr));
							p_frame->can_dlc = sizeof(addr);
							proto_state = ST_BOOT_SET_WRITE_ADDR;
							ret_val = 1U;
						}
					}else{
						printf("error while erasing 1\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_SET_WRITE_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						printf("set write address OK\n");
						printf("start write\n");
						size_t bytes_read = fread(rcv_data, 1, RW_SIZE, f_read);

						crc32 = CRC_INITIALVALUE;
						crc32 = stm32_sw_crc32_by_byte(crc32, rcv_data, bytes_read);

						p_frame->can_id = caddr + CMD_WRITE;
						memcpy(p_frame->data, rcv_data, bytes_read);
						p_frame->can_dlc = bytes_read;
						addr = bytes_read;
						proto_state = ST_BOOT_WRITE;
						ret_val = 1U;
					}else{
						printf("error while erasing 2\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_WRITE:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_WRITE)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						if(addr < f_len)
						{
							++cmd_snd_cnt;
							if(cmd_snd_cnt == 64)
							{
								fprintf(stderr, ".");
								cmd_snd_cnt = 0;
							}
							size_t bytes_read = fread(rcv_data, 1, RW_SIZE, f_read);

							crc32 = stm32_sw_crc32_by_byte(crc32, rcv_data, bytes_read);

							p_frame->can_id = caddr + CMD_WRITE;
							memcpy(p_frame->data, rcv_data, bytes_read);
							p_frame->can_dlc = bytes_read;
							addr += bytes_read;
							proto_state = ST_BOOT_WRITE;
							ret_val = 1U;
						}else{
							cmd_snd_cnt = 0;
							printf("\n");
							printf("write CRC32, crc32 = 0x%08X\n", crc32);
							p_frame->can_id = caddr + CMD_WRITE;
							memcpy(p_frame->data, &crc32, sizeof(crc32));
							p_frame->can_dlc = sizeof(crc32);
							proto_state = ST_BOOT_WRITE_CRC32;
							ret_val = 1U;
						}
					}else{
						printf("error while writing 1\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_WRITE_CRC32:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_WRITE)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						uint32_t res = fseek(f_read, 0, SEEK_SET);
						if(res)
						{
							printf("can not seek position 3\n");
							ret_val = 2U;
						}else{
							printf("set start read address\n");
							addr = START_APP_ADDR;
							p_frame->can_id = caddr + CMD_SET_ADDR;
							memcpy(p_frame->data, &addr, sizeof(addr));
							p_frame->can_dlc = sizeof(addr);
							proto_state = ST_BOOT_SET_READ_ADDR;
							ret_val = 1U;
						}
					}else{
						printf("error while writing CRC32\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_SET_READ_ADDR:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_SET_ADDR)
				{
					memcpy(rcv_data, p_frame->data, RW_SIZE);
					t_std_answer* p_answ = (t_std_answer*)rcv_data;
					if(p_answ->_err == ERR_OK)
					{
						printf("start verify\n");
						p_frame->can_id = caddr + CMD_READ;
						p_frame->can_dlc = 0;
						proto_state = ST_BOOT_READ;
						addr = 0;
						ret_val = 1U;
					}else{
						printf("error while writing 2\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_READ:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_READ)
				{
					if(p_frame->can_dlc)
					{
						++cmd_snd_cnt;
						if(cmd_snd_cnt == 64)
						{
							fprintf(stderr, ".");
							cmd_snd_cnt = 0;
						}

						memcpy(rcv_data, p_frame->data, p_frame->can_dlc);
						uint8_t rd_data[RW_SIZE];
						addr += p_frame->can_dlc;
						size_t bytes_read = fread(rd_data, p_frame->can_dlc, 1, f_read);

						int32_t res = memcmp(rcv_data, rd_data, bytes_read);
						if(res == 0)
						{
							p_frame->can_id = caddr + CMD_READ;
							p_frame->can_dlc = 0;
							ret_val = 1U;
							if(addr == f_len)
							{
								printf("\n");
								printf("verify CRC32\n");

								proto_state = ST_BOOT_READ_CRC32;
							}
						}else{
							printf("verify error at adddress - 0x%08X\n", (addr + START_APP_ADDR));
							ret_val = 2U;
						}
					}else{
						printf("verify breaking\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_READ_CRC32:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_READ)
				{
					memcpy(&rd_crc32, p_frame->data, sizeof(rd_crc32));

					if(rd_crc32 == crc32)
					{
						printf("verify OK\n");
						printf("now go to APP\n");
						proto_state = ST_BOOT_TO_APP;
						p_frame->can_id = caddr + CMD_GO_APP;
						p_frame->can_dlc = 0;
						ret_val = 1U;
					}else{
						printf("verify error CRC32\n");
						ret_val = 2U;
					}
				}
			}
			break;
		case ST_BOOT_TO_APP:
			{
				uint32_t cmd = p_frame->can_id & CAN_CMD_MASK;
				if(cmd == CMD_GO_APP)
				{
					printf("go app cmd successful\n");
					sleep(1);
					p_frame->can_id = caddr + CMD_GET_STATE;
					p_frame->can_dlc = 0;

					proto_state = ST_BOOT_GET_STATE_AFTER_UPDATE;

					ret_val = 1U;
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

						ret_val = 3U;
					}
				}
			}
			break;
	}

	return ret_val;
}

void can_protocol_close_file(void)
{
	if(f_read)
	{
		fclose(f_read);
	}
}

//------------------- End of File ------------------
