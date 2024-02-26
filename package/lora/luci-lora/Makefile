#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-lora
PKG_VERSION:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/lora/luci-lora.git
PKG_SOURCE_VERSION:=30fbbc7329a4dcdb8c7d98a9b2f925dcc2838a47

include $(INCLUDE_DIR)/package.mk

define Package/luci-lora
  SECTION:=LORA
  CATEGORY:=LoraWAN Packages
  SUBMENU:=LoRaWAN
  TITLE:=LuCi web-interface for lorawan configuration.
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
  DEPENDS:= +packet-forwarder
endef

define Package/luci-lora/description
 Default configuration applied on first boot and reset.
 This package contains deaults for main router OpenVPN.
endef

define Build/Compile
	echo "Nothing to compile";
endef

define Package/luci-lora/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/ 
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller/
	$(CP) $(PKG_BUILD_DIR)/controller/* $(1)/usr/lib/lua/luci/controller/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/
	$(CP) $(PKG_BUILD_DIR)/view/* $(1)/usr/lib/lua/luci/view/
endef

$(eval $(call BuildPackage,luci-lora))
