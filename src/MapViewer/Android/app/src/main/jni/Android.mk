LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := GisLibrary

STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
COMMON_LIB_PATH := ..\..\..\..\..\CommonLibrary

EMBDB_I_PATH := ..\..\..\..\..\EmbDB
EMBDB_LIB_PATH := ..\..\..\..\..\EmbDB

LOCAL_CFLAGS :=	-I. \
		-Istlport \
		-DANDROID \
		-I$(STLPORT_BASE)/stlport \
		-I$(COMMON_I_PATH) \
		-I$(EMBDB_I_PATH) \
		-x c++

LOCAL_STATIC_LIBRARIES :=libcommon libEmbDB




LOCAL_SRC_FILES := GisLibrary.cpp \
				   map.cpp



include $(BUILD_SHARED_LIBRARY)
$(call import-add-path, $(COMMON_LIB_PATH))
$(call import-module,CommonLibrary)

$(call import-add-path, $(EMBDB_LIB_PATH))
$(call import-module,EmbDB)

$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)

