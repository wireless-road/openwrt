#!/bin/sh /etc/rc.common
# Copyright (C) 2007-2011 OpenWrt.org

python /usr/bin/esptool.pyc -p /dev/ttymxc2 -b 960000 --before default_reset \
write_flash --flash_mode dio --flash_freq 40m --flash_size detect \
0x8000 esp_hosted_partition-table_esp32_spi_uart_v0.4.bin \
0x1000 esp_hosted_bootloader_esp32_spi_uart_v0.4.bin \
0x10000 esp_hosted_firmware_esp32_spi_uart_v0.4.bin