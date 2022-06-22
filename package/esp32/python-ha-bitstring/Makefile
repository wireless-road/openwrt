#
# Copyright (C) 2021 @lmahmutov (https://smartgateway.ru/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-ha-bitstring
PKG_VERSION:=3.1.9
PKG_RELEASE:=1

PYPI_NAME:=bitstring
PKG_HASH:=a5848a3f63111785224dca8bb4c0a75b62ecdef56a042c8d6be74b16f7e860e7

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Lenar Mahmutov <lenz1986@gmail.com>

include $(TOPDIR)/feeds/packages/lang/python/pypi.mk
include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/python/python3-package.mk

define Package/python3-ha-bitstring
  CATEGORY:=ESP32
  TITLE:=Simple construction, analysis and modification of binary data.
  URL:=https://pypi.org/project/bitstring/
  DEPENDS+= \
      +python3-light 
endef

define Package/python3-ha-bitstring/description
A Python-based, open-source, platform-independent utility to communicate with the ROM bootloader in Espressif chips.
endef

$(eval $(call Py3Package,python3-ha-bitstring))
$(eval $(call BuildPackage,python3-ha-bitstring))