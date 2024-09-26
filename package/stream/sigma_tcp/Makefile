#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sigma_tcp
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/sigma_tcp
  SECTION:=Stream IMX
  CATEGORY:=Stream IMX
  DEPENDS:=
  TITLE:=SigmaStudio TCP utility for linux.
  MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
endef

TARGET_CPPFLAGS :=     \
	-I$(PKG_BUILD_DIR) \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/*.c $(PKG_BUILD_DIR)/
	$(CP) ./src/*.h $(PKG_BUILD_DIR)/
	$(CP) ./src/Makefile $(PKG_BUILD_DIR)/
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS)" $(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
endef

define Package/sigma_tcp/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sigma_tcp $(1)/sbin/sigma_tcp
#	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,sigma_tcp))
