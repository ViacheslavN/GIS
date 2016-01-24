APP_MODULES := GisLibrary
APP_ABI := armeabi-v7a arm64-v8a

APP_STL := stlport_static
#APP_STL := gnustl_static
#APP_CFLAGS := --std=c++11
#NDK_TOOLCHAIN_VERSION := 4.9


ifeq ($(NDK_DEBUG),1)
  APP_OPTIM := debug
  APP_CFLAGS += -DDEBUG -D_DEBUG
else
  APP_OPTIM := release
  APP_CFLAGS += -DRELEASE -D_RELEASE
endif