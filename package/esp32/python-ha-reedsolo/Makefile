#
# Copyright (C) 2021 @lmahmutov (https://smartgateway.ru/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-ha-reedsolo
PKG_VERSION:=1.5.4
PKG_RELEASE:=1

PYPI_NAME:=reedsolo
PKG_HASH:=b8b25cdc83478ccb06361a0e8fadc27b376a3dfabbb1dc6bb583a998a22c0127

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Lenar Mahmutov <lenz1986@gmail.com>

include $(TOPDIR)/feeds/packages/lang/python/pypi.mk
include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/python/python3-package.mk

define Package/python3-ha-reedsolo
  CATEGORY:=ESP32
  TITLE:=Pure-Python Reed Solomon encoder/decoder.
  URL:=https://pypi.org/project/reedsolo/
  DEPENDS+= \
      +python3-light 
endef

define Package/python3-ha-reedsolo/description
A pure-python universal errors-and-erasures Reed-Solomon Codec , based on the wonderful tutorial at wikiversity, written by “Bobmath” and “LRQ3000”.
endef

$(eval $(call Py3Package,python3-ha-reedsolo))
$(eval $(call BuildPackage,python3-ha-reedsolo))