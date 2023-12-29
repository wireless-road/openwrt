#
# Copyright (C) 2023 WirelessRoad
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=packet-forwarder
PKG_VERSION:=4.0.1
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/lora/packet-forwarder.git
PKG_SOURCE_VERSION:=d475b730fcbba5f28fa574f1d5f44969e0ea04f3
PKG_MAINTAINER:=Sergey Moryakov <nquest@nqnet.org>
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/packet-forwarder/Default
  TITLE:=Semtech packet-forwarder program
  URL:=https://github.com/Lora-net/packet_forwarder
endef

define Package/packet-forwarder
$(call Package/packet-forwarder/Default)
  SECTION:=LORA
  CATEGORY:=WirelessRoad Apps
  SUBMENU:=LoRaWAN
  DEPENDS:=+librt
endef

define Package/packet-forwarder/description
  A LoRa packet forwarder is a program running on the host of a LoRa gateway
  that forwards RF packets receive by the concentrator to a server through a
  IP/UDP link, and emits RF packets that are sent by the server.
endef

define Build/Compile
  $(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR)/libloragw
	$(MAKE) CC=$(TARGET_CC) LGW_PATH=../libloragw -C $(PKG_BUILD_DIR)/lora_pkt_fwd
endef

define Package/packet-forwarder/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lora_pkt_fwd/lora_pkt_fwd $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lora_pkt_fwd.init $(1)/etc/init.d/lora_pkt_fwd
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/global_conf.json $(1)/etc/global_conf.json
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/lora.config $(1)/etc/config/lora
endef

$(eval $(call BuildPackage,packet-forwarder))

