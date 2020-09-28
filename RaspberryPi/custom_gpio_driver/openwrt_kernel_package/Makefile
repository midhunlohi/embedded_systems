#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-custom
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-custom
  SUBMENU:=GPIO Driver for RPI
  TITLE:= GPIO Driver for RPI
  FILES:=$(PKG_BUILD_DIR)/gpio_driver.ko
  KCONFIG:=
endef

define KernelPackage/gpio-custom/description
 Kernel module for register a custom GPIO device driver for LED
endef

EXTRA_KCONFIG:= \
	CONFIG_GPIO_CUSTOM=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,gpio-custom))
