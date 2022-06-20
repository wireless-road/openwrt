include $(TOPDIR)/rules.mk

PKG_NAME:=esp32-firmware
PKG_VERSION:=1.0.0
PKG_RELEASE:=$(AUTORELEASE)

PKG_MAINTAINER:=Lenar Mahmutov <lenz1986@gmail.com>
PKG_LICENSE:=CC0-1.0

include $(INCLUDE_DIR)/package.mk

define Package/esp32-firmware
	CATEGORY:=ESP32
	TITLE:=firmware for ESP32.
endef

define Package/esp32-firmware/description
	Esp32 firmware flash script and init.d.
endef

Build/Prepare:=:
Build/Configure:=:
Build/Compile:=:

define Package/esp32-firmware/install

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/esp32_ble $(1)/etc/init.d

	$(INSTALL_DIR) $(1)/root/esp32
	$(INSTALL_BIN) ./files/flash_esp.sh $(1)/root/esp32/
	$(INSTALL_BIN) ./files/esp_hosted_bootloader_esp32_spi_uart_v0.4.bin $(1)/root/esp32/
	$(INSTALL_BIN) ./files/esp_hosted_firmware_esp32_spi_uart_v0.4.bin $(1)/root/esp32/
	$(INSTALL_BIN) ./files/esp_hosted_ota_data_initial_esp32_spi_uart_v0.4.bin $(1)/root/esp32/
	$(INSTALL_BIN) ./files/esp_hosted_partition-table_esp32_spi_uart_v0.4.bin $(1)/root/esp32/
endef

$(eval $(call BuildPackage,esp32-firmware))