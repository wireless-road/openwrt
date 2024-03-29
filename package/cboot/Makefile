include $(TOPDIR)/rules.mk

PKG_NAME:=cboot
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.m2m-tele.com/openwrt-packages/cboot.git
PKG_SOURCE_VERSION:=e58147cb184280725ed9d6da75cb031cb6acb4cc

PKG_BUILD_DIR:=$(BUILD_DIR)/cboot-$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define Package/cboot
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=cboot
endef

define Package/cboot/description
  Firmware updater via CAN bus
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/cboot/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/cboot $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/cboot.conf $(1)/etc/cboot.json
endef

$(eval $(call BuildPackage,cboot))

