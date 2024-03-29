#
# Copyright (C) 2008-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gst1-plugins-base-alexa
PKG_VERSION:=1.16.2
PKG_RELEASE:=1

PKG_SOURCE:=gst-plugins-base-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://gstreamer.freedesktop.org/src/gst-plugins-base
PKG_HASH:=b13e73e2fe74a4166552f9577c3dcb24bed077021b9c7fa600d910ec6987816a
PKG_BUILD_DIR:=$(BUILD_DIR)/gst-plugins-base-$(PKG_VERSION)

#PKG_MAINTAINER:=W. Michael Petullo <mike@flyn.org> \
#		Ted Hess <thess@kitschensync.net>
PKG_LICENSE:=LGPL-2.1-or-later GPL-2.0-or-later
PKG_LICENSE_FILES:=COPYING.LIB COPYING
PKG_CPE_ID:=cpe:/a:gstreamer:gst-plugins-base-alexa

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk
#include ../../devel/meson/meson.mk
include ../../feeds/packages/meson/meson.mk

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_gst1-alexa-mod-alsa \
	CONFIG_PACKAGE_gst1-alexa-mod-app \
	CONFIG_PACKAGE_gst1-alexa-mod-audioconvert \
	CONFIG_PACKAGE_gst1-alexa-mod-audiorate \
	CONFIG_PACKAGE_gst1-alexa-mod-audioresample \
	CONFIG_PACKAGE_gst1-alexa-mod-audiotestsrc \
	CONFIG_PACKAGE_gst1-alexa-mod-playback \
	CONFIG_PACKAGE_gst1-alexa-mod-gio \
	CONFIG_PACKAGE_gst1-alexa-mod-ogg \
	CONFIG_PACKAGE_gst1-alexa-mod-opus \
	CONFIG_PACKAGE_gst1-alexa-mod-tcp \
	CONFIG_PACKAGE_gst1-alexa-mod-theora \
	CONFIG_PACKAGE_gst1-alexa-mod-typefind \
	CONFIG_PACKAGE_gst1-alexa-mod-videoconvert \
	CONFIG_PACKAGE_gst1-alexa-mod-videorate \
	CONFIG_PACKAGE_gst1-alexa-mod-videoscale \
	CONFIG_PACKAGE_gst1-alexa-mod-videotestsrc \
	CONFIG_PACKAGE_gst1-alexa-mod-volume \
	CONFIG_PACKAGE_gst1-alexa-mod-vorbis \
	CONFIG_PACKAGE_libgst1app-alexa

GST_BASE_LIBS:=
GST_BASE_MODULES:=

define Package/gstreamer1-base-alexa/Default
  CATEGORY:=Multimedia
  SECTION:=multimedia
  TITLE:=GStreamer2 Alexa
  URL:=https://gstreamer.freedesktop.org/
  DEPENDS:= +libgstreamer1 $(ICONV_DEPENDS)
endef

define Package/gstreamer1-base-alexa/description/Default
 GStreamer open source multimedia framework (Alexa)
endef


define Package/gst1-plugins-base-alexa
$(call Package/gstreamer1-base-alexa/Default)
  TITLE+= plugins collection (base)
  DEPENDS+= $(GST_DEPENDS)
  HIDDEN:=1
endef

define Package/gts1-plugins-base-alexa/description
$(call Package/gstreamer1-base-alexa/description/Default)
 .
 This meta package contains only dependencies to the other libraries and
 plugins from the base plugins collection. (Alexa)
endef

define Package/gstreamer1-plugins-base-alexa
  $(call Package/gstreamer1-base-alexa/Default)
  TITLE+= plugins collection (base) (Alexa)
  DEPENDS:=+ALL:gst1-plugins-base-alexa +gstreamer1-libs
endef

define Package/gstreamer1-plugins-base-alexa/config
    menu "Select GStreamer base modules and libraries (Alexa)"
      depends on PACKAGE_gstreamer1-plugins-base-alexa


    config gst1-plugins-base-alexa-all
      bool "Include all GStreamer base plugins (Alexa)"
      select PACKAGE_gst1-plugins-base-alexa

    comment "Modules"

   $(foreach mod,$(GST_BASE_MODULES), \
    config PACKAGE_gst1-alexa-mod-$(mod)
      prompt "GStreamer $(mod) module (Alexa)"

   )

    comment "Libraries"

   $(foreach lib,$(GST_BASE_LIBS), \
    config PACKAGE_libgst1$(lib)
      prompt "GStreamer $(lib) library (Alexa)"

   )

  endmenu

endef

GST_COND_SELECT = -D$(1)=$(if $(CONFIG_PACKAGE_gst1-alexa-mod-$(1)),en,dis)abled

GST_VERSION:=1.0

MESON_ARGS += \
	-Daudioresample_format=int \
	-Dgl=disabled \
	\
	$(call GST_COND_SELECT,adder) \
	$(call GST_COND_SELECT,app) \
	$(call GST_COND_SELECT,audioconvert) \
	$(call GST_COND_SELECT,audiomixer) \
	$(call GST_COND_SELECT,audiorate) \
	$(call GST_COND_SELECT,audioresample) \
	$(call GST_COND_SELECT,audiotestsrc) \
	$(call GST_COND_SELECT,compositor) \
	$(call GST_COND_SELECT,encoding) \
	$(call GST_COND_SELECT,gio) \
	$(call GST_COND_SELECT,overlaycomposition) \
	$(call GST_COND_SELECT,pbtypes) \
	$(call GST_COND_SELECT,playback) \
	$(call GST_COND_SELECT,rawparse) \
	$(call GST_COND_SELECT,subparse) \
	$(call GST_COND_SELECT,tcp) \
	-Dtypefind=$(if $(CONFIG_PACKAGE_gst1-alexa-mod-typefindfunctions),en,dis)abled \
	$(call GST_COND_SELECT,videoconvert) \
	$(call GST_COND_SELECT,videorate) \
	$(call GST_COND_SELECT,videoscale) \
	$(call GST_COND_SELECT,videotestsrc) \
	$(call GST_COND_SELECT,volume) \
	\
	$(call GST_COND_SELECT,alsa) \
	$(call GST_COND_SELECT,cdparanoia) \
	$(call GST_COND_SELECT,ogg) \
	$(call GST_COND_SELECT,opus) \
	$(call GST_COND_SELECT,pango) \
	$(call GST_COND_SELECT,theora) \
	$(call GST_COND_SELECT,vorbis) \
	-Dx11=disabled \
	-Dxshm=disabled \
	-Dxvideo=disabled \
	\
	-Dexamples=disabled \
	-Dtests=disabled \
	-Dtools=disabled \
	-Dgtk_doc=disabled \
	-Dintrospection=disabled \
	-Dnls=enabled \
	-Dorc=disabled \
	-Dgobject-cast-checks=disabled \
	-Dglib-asserts=disabled \
	-Dglib-checks=disabled \
	-Dgst_debug=true 

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/include/gstreamer-$(GST_VERSION)/* \
		$(1)/usr/include/gstreamer-$(GST_VERSION)/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/libgst*-$(GST_VERSION).so* \
		$(1)/usr/lib/ \
	)
#	$(INSTALL_DIR) $(1)/usr/lib/gstreamer-$(GST_VERSION)
#	( cd $(PKG_INSTALL_DIR); $(CP) \
#		./usr/lib/gstreamer-$(GST_VERSION)/libgst*.so \
#		$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
#	)
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/pkgconfig/gstreamer*-$(GST_VERSION).pc \
		$(1)/usr/lib/pkgconfig/ \
	)
endef


define Package/gst1-plugins-base-alexa/install
  true
endef

define Package/gstreamer1-plugins-base-alexa/install
  true
endef

# 1: short name
# 2: description
# 3: dependencies on other gstreamer libraries (short name)
# 4: dependencies on other packages
define GstBuildLibrary

  GST_DEPENDS += +libgst1$(1)-alexa

  GST_BASE_LIBS+= $(1)-alexa

  define Package/libgst1$(1)-alexa
    $(call Package/gstreamer1-base-alexa/Default)
    TITLE+= $(2) library (base)
    DEPENDS+= $$(foreach p,$(3),+libgst1$$(p)-alexa) $(4)
    HIDDEN:=1
  endef

  define Package/libgst1$(1)-alexa/description
   $(call Package/gstreamer1-base-alexa/description/Default)
   .
   This package contains the GStreamer $(2) library.
  endef

  define Package/libgst1$(1)-alexa/install
	$(INSTALL_DIR) $$(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/libgst$(1)-$(GST_VERSION).so.* \
		$$(1)/usr/lib/ \
	)
  endef

  $$(eval $$(call BuildPackage,libgst1$(1)-alexa))
endef

$(eval $(call GstBuildLibrary,allocators,allocators,,))
$(eval $(call GstBuildLibrary,app,app,,))
$(eval $(call GstBuildLibrary,audio,audio,tag,))
$(eval $(call GstBuildLibrary,fft,FFT,,))
$(eval $(call GstBuildLibrary,pbutils,utils,audio tag video,))
$(eval $(call GstBuildLibrary,riff,RIFF media,audio tag,,))
$(eval $(call GstBuildLibrary,rtp,RTP,,))
$(eval $(call GstBuildLibrary,rtsp,RTSP,,))
$(eval $(call GstBuildLibrary,sdp,SDP,rtp,))
$(eval $(call GstBuildLibrary,tag,tag support,,))
$(eval $(call GstBuildLibrary,video,video,,))

# 1: short name
# 2: description
# 3: dependencies on other gstreamer libraries (short name)
# 4: dependencies on other gstreamer plugins (short name)
# 5: dependencies on other packages
define GstBuildPlugin

  GST_DEPENDS += +gst1-alexa-mod-$(1)

  GST_BASE_MODULES+= $(1)

  define Package/gst1-alexa-mod-$(1)
    $(call Package/gstreamer1-base-alexa/Default)
    TITLE+= $(2) plugin (base)
    DEPENDS+= $$(foreach p,$(3),+libgst1$$(p)-alexa) $$(foreach p,$(4),+gst1-alexa-mod-$$(p)) $(5)
    HIDDEN:=1
  endef

  define Package/gst1-alexa-mod-$(1)/description
   $(call Package/gstreamer1-base-alexa/description/Default)
   .
   This package contains the GStreamer $(2) plugin.
  endef

  define Package/gst1-alexa-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/gstreamer-$(GST_VERSION)/libgst$(1).so* \
		$$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
	)
  endef

  $$(eval $$(call BuildPackage,gst1-alexa-mod-$(1)))
endef

$(eval $(call GstBuildPlugin,alsa,ALSA,audio tag,,+alsa-lib))
$(eval $(call GstBuildPlugin,app,app,app,,))
$(eval $(call GstBuildPlugin,audioconvert,audio format conversion,audio,,))
$(eval $(call GstBuildPlugin,audiorate,audio rate adjusting,audio tag,,))
$(eval $(call GstBuildPlugin,audioresample,audio resampling,audio tag,,))
$(eval $(call GstBuildPlugin,audiotestsrc,audio test,audio tag,,+libgst1controller))
$(eval $(call GstBuildPlugin,playback,media decoder (v2),pbutils,,))
$(eval $(call GstBuildPlugin,gio,GIO,,,))
$(eval $(call GstBuildPlugin,ogg,Ogg,riff tag pbutils video,,+libogg))
$(eval $(call GstBuildPlugin,opus,OPUS plugin library,pbutils video,,+libopus))
$(eval $(call GstBuildPlugin,tcp,TCP,,,+libgst1net))
$(eval $(call GstBuildPlugin,theora,Theora,tag video,,+libogg +libtheora))
$(eval $(call GstBuildPlugin,typefindfunctions,'typefind' functions,audio pbutils tag video,,))
$(eval $(call GstBuildPlugin,videoconvert,video format conversion,video,,))
$(eval $(call GstBuildPlugin,videorate,Adjusts video frames,video,,))
$(eval $(call GstBuildPlugin,videoscale,Resizes video,video,,))
$(eval $(call GstBuildPlugin,videotestsrc,video test,video,,+liboil))
$(eval $(call GstBuildPlugin,volume,volume,audio,,+liboil +libgst1controller))
$(eval $(call GstBuildPlugin,vorbis,Vorbis,audio tag,ogg,+libvorbis))

$(eval $(call BuildPackage,gstreamer1-plugins-base-alexa))
$(eval $(call BuildPackage,gst1-plugins-base-alexa))
