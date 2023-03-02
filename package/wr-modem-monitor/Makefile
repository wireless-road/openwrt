include $(TOPDIR)/rules.mk

PKG_NAME:=wr-modem-monitor
PKG_VERSION:=2023.03.01
PKG_RELEASE=1

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>

include $(INCLUDE_DIR)/package.mk

define Package/wr-modem-monitor
  SECTION:=Utilities
  CATEGORY:=Wireless-Road
  TITLE:=Generic modem init and watchdog script
endef

define Build/Compile
	true
endef

define Package/wr-modem-monitor/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mdmmon.init $(1)/etc/init.d/mdmmon
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) ./files/mdmmon $(1)/usr/bin/mdmmon
endef

$(eval $(call BuildPackage,wr-modem-monitor))
