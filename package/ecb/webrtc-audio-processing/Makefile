#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=webrtc-audio-processing
PKG_VERSION:=0.3.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://freedesktop.org/software/pulseaudio/webrtc-audio-processing/
PKG_HASH:=4dabdd0789acd117d88d53f0a793cf4e906c6a93ee9aa6975ec928eafbf1dfe5

PKG_MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=COPYING

# PKG_BUILD_DEPENDS:=meson/host
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
# include ../../../feeds/packages/devel/meson/meson.mk

# MESON_ARGS+=-Diconv=disabled -Ddocumentation=disabled

define Package/webrtc-audio-processing
  SECTION:=ECB
  CATEGORY:=ECB
  SUBMENU:=Libraries
  TITLE:=AudioProcessing module from the  WebRTC  project
  URL:=https://freedesktop.org/software/pulseaudio/webrtc-audio-processing/
  DEPENDS:=+abseil-cpp
endef

define Package/webrtc-audio-processing/description
	This is meant to be a more Linux packaging
	friendly copy of the AudioProcessing module
	from the WebRTC project.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig/
endef

define Package/webrtc-audio-processing/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,webrtc-audio-processing))
