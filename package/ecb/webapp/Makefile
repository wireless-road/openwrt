#
# Copyright (C) 2022 Sergey Moryakov
#
# This is free software, licensed under the MIT License.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ecb-webapp
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ecb-webapp
  SECTION:=ECB
  CATEGORY:=ECB
  DEPENDS:=+uhttpd \
  			+php7 \
			+php7-cgi \
			+php7-mod-calendar \
			+php7-mod-fileinfo \
			+php7-mod-session \
			+php7-mod-sockets
  TITLE:=ECB Web control application
  MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/ecb-webapp/install
	$(INSTALL_DIR) $(1)/www
	$(CP) ./src/www/* $(1)/www/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) ./files/* $(1)/

endef

$(eval $(call BuildPackage,ecb-webapp))