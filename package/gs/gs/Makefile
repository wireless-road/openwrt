include $(TOPDIR)/rules.mk

PKG_NAME:=gs
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/gs-$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define Package/gs
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=gs control application
  DEPENDS:=+libmodbus \
	+libgpiod
endef

define Package/gs/description
  GS demo for getting info
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef
TARGET_LDFLAGS += -lmodbus -lgpiod
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/gs/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gs $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/gpio_conf.json $(1)/etc/gpio_conf.json
endef

$(eval $(call BuildPackage,gs))
