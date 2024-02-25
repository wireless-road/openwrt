#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-gs
PKG_VERSION:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/gs/luci-gs.git
PKG_SOURCE_VERSION:=7655f16d9ab954b860ebae9e4fcb5e07f22a9b05

include $(INCLUDE_DIR)/package.mk

define Package/luci-gs
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=LuCi web-interface for gs configuration.
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/luci-gs/description
 Default configuration applied on first boot and reset.
endef

define Build/Compile
	echo "Nothing to compile";
endef

define Package/luci-gs/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/
	$(CP) $(PKG_BUILD_DIR)/luci/* $(1)/usr/lib/lua/luci/
	$(INSTALL_DIR) $(1)/www/luci-static/resources/
	$(CP) $(PKG_BUILD_DIR)/styles/* $(1)/www/luci-static/resources/
endef

$(eval $(call BuildPackage,luci-gs))


