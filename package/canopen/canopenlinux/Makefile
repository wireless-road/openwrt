#
# Copyright (C) 2023 Sergey Moryakov <sergey@nqnet.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=canopenlinux
PKG_VERSION:=4.0
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/CANopenNode/CANopenLinux.git
PKG_SOURCE_DATE:=2021-05-06
PKG_SOURCE_VERSION:=ebcc3c8f01c9360c3a090267611e805bc78bd236
PKG_MIRROR_HASH:=59816df6f080689a112cc124a1df2b66419fb0ab9f3d08a725e3f6752482eee8

include $(INCLUDE_DIR)/package.mk
RSTRIP:=:


define Package/canopenlinux
	SECTION:=WR CAN
	CATEGORY:=WR CAN
	DEPENDS:=
	TITLE:=CANopenLinux
	MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
endef

define Package/canopenlinux/description
	CANopenLinux is a CANopen stack running on Linux devices.
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS)" $(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	CFLAGS="$(TARGET_CFLAGS)" $(MAKE) -C $(PKG_BUILD_DIR)/cocomm $(TARGET_CONFIGURE_OPTS)
endef

define Package/canopenlinux/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/canopend $(1)/usr/sbin/canopend
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cocomm/cocomm $(1)/usr/sbin/cocomm
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/*.sh $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/init.d/canopend $(1)/etc/init.d/canopend
endef

$(eval $(call BuildPackage,canopenlinux))
