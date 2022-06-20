include $(TOPDIR)/rules.mk

PKG_NAME:=cboot
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/cboot-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/cboot
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=c-boot driving example
endef

define Package/c-boot/description
  GPIO input/output driving example
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/c-boot/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/c-boot $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/c-bootn.conf $(1)/etc/cboot.json
endef

$(eval $(call BuildPackage,cboot))
