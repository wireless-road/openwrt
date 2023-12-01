#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=codesys
PKG_VERSION:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/codesys.git
PKG_SOURCE_VERSION:=8c829ad4e60694f081fbece800b5946495d19fee

include $(INCLUDE_DIR)/package.mk

define Package/codesys
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=LuCi web-interface example.
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/codesys/description
 Default configuration applied on first boot and reset.
endef

define Build/Compile
	echo "Nothing to compile";
endef

define Package/codesys/install
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/default/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DIR) $(1)/opt/
	$(INSTALL_DIR) $(1)/opt/codesys/
	$(INSTALL_DIR) $(1)/opt/codesys/bin/
	$(INSTALL_DIR) $(1)/opt/codesys/lib/
	$(INSTALL_DIR) $(1)/opt/codesys/scripts/
	$(INSTALL_DIR) $(1)/opt/var/opt/codesys/
	$(INSTALL_DIR) $(1)/share/doc/codesyscontrol/
	$(CP) $(PKG_BUILD_DIR)/etc/* $(1)/etc/
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/lib/
	$(CP) $(PKG_BUILD_DIR)/opt/* $(1)/opt/
	$(CP) $(PKG_BUILD_DIR)/usr/* $(1)/usr/
endef

$(eval $(call BuildPackage,codesys))




