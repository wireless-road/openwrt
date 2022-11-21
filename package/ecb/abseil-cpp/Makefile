include $(TOPDIR)/rules.mk

PKG_NAME:=abseil-cpp
PKG_VERSION:=20210324.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/abseil/abseil-cpp/archive/refs/tags/
PKG_HASH:=59b862f50e710277f8ede96f083a5bb8d7c9595376146838b9580be90374ee1f

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/abseil-cpp-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/abseil-cpp-$(PKG_VERSION)

PKG_MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=LICENSE

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/abseil-cpp
  SECTION:=ECB
  CATEGORY:=ECB
  SUBMENU:=Libraries
  TITLE:=Abseil - C++ Common Libraries
  URL:=https://github.com/abseil/abseil-cpp
  DEPENDS:=+libstdcpp
endef

define Package/abseil-cpp/description
  Abseil - C++ Common Libraries
endef

CMAKE_OPTIONS+= \
		-DCMAKE_CXX_STANDARD=17 \
        -DABSL_CXX_STANDARD=17 \
        -DABSL_USE_GOOGLETEST_HEAD=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DABSL_ENABLE_INSTALL=ON \
        -DCMAKE_MODULE_LINKER_FLAGS=\"-Wl,--no-undefined -Wl,--no-undefined\" \
        -DBUILD_SHARED_LIBS=ON

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/absl $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig/
	$(INSTALL_DIR) $(1)/usr/lib/cmake
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake/* $(1)/usr/lib/cmake
endef

define Package/abseil-cpp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,abseil-cpp))
$(eval $(call HostBuild))
