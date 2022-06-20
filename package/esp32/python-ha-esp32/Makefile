#
# Copyright (C) 2021 @lmahmutov (https://smartgateway.ru/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-ha-esptool
PKG_VERSION:=4.1
PKG_RELEASE:=1

PYPI_NAME:=esptool
PKG_HASH:=51a41655187780cb802a5827fab68a3db0d6206330560459c55fcf0e222f277f

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Lenar Mahmutov <lenz1986@gmail.com>

include $(TOPDIR)/feeds/packages/lang/python/pypi.mk
include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/python/python3-package.mk

define Package/python3-ha-esptool
  CATEGORY:=ESP32
  TITLE:=Utility to communicate & flash ESP32.
  URL:=https://pypi.org/project/esptool/
  DEPENDS+= \
      +python3-light \
      +python3-ha-bitstring \
      +python3-ha-cryptography \
      +python3-ha-ecdsa \
      +python3-ha-pyserial \
      +python3-ha-reedsolo
endef

define Package/python3-ha-esptool/description
 A Python-based, open-source, platform-independent utility to communicate with the ROM bootloader in Espressif chips.
endef

define Py3Package/python3-ha-esptool/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/esp32_ble $(1)/etc/init.d

	$(INSTALL_DIR) $(1)/root
	$(INSTALL_BIN) ./files/flash_esp.sh $(1)/root/
endef


$(eval $(call Py3Package,python3-ha-esptool))
$(eval $(call BuildPackage,python3-ha-esptool))