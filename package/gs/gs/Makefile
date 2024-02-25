include $(TOPDIR)/rules.mk

PKG_NAME:=gs
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/gs/gs.git
PKG_SOURCE_VERSION:=1446ade5645e72f10c2177b6bf35e8c9d7daff4c

PKG_BUILD_DIR:=$(BUILD_DIR)/gs-$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define Package/gs
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=gs control application
  DEPENDS:=+libmodbus
endef

define Package/gs/description
  GS demo for getting info
endef

# uncomment following to simplify core dumps parsing
# TARGET_CFLAGS += -gdwarf -g3
TARGET_LDFLAGS += -lmodbus
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/gs/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/gs $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/test_modbus $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/test_modbus2 $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/test_can $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/gpio_conf.json $(1)/etc/gpio_conf.json
	$(INSTALL_DIR) $(1)/etc/gs
	$(INSTALL_DIR) $(1)/etc/gs/1
	$(INSTALL_DIR) $(1)/etc/gs/2
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/gs/1/* $(1)/etc/gs/1
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/gs/2/* $(1)/etc/gs/2
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/init.d/gs $(1)/etc/init.d/gs
	chmod 0750 $(1)/etc/init.d/gs
endef

$(eval $(call BuildPackage,gs))

