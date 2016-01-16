LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := GisLibrary

STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
COMMON_LIB_PATH := ..\..\..\..\..\CommonLibrary

EMBDB_I_PATH := ..\..\..\..\..\EmbDB
EMBDB_LIB_PATH := ..\..\..\..\..\EmbDB


FREETYPE_I_PATH := ..\..\..\..\..\ThirdParty\freetype\include
PROJ4_I_PATH := ..\..\..\..\..\ThirdParty\proj4\src
SHAPELIB_I_PATH := ..\..\..\..\..\ThirdParty\ShapeLib
ZLIB_I_PATH := ..\..\..\..\..\ThirdParty\ShapeLib
PNG_I_PATH := ..\..\..\..\..\ThirdParty\PNG
JPEG_I_PATH := ..\..\..\..\..\ThirdParty\jpeg

FHIRD_PARTY_LIB_PATH := ..\..\..\..\..\ThirdParty

LOCAL_CFLAGS :=	-I. \
		-Istlport \
		-DANDROID \
		-I$(STLPORT_BASE)/stlport \
		-I$(COMMON_I_PATH) \
		-I$(EMBDB_I_PATH) \
		-I$(FREETYPE_I_PATH) \
		-I$(PROJ4_I_PATH) \
		-I$(SHAPELIB_I_PATH) \
		-I$(PNG_I_PATH) \
		-x c++

LOCAL_STATIC_LIBRARIES :=libcommon libEmbDB libfreetype libproj4 libshapelib libzlib libpng libjpeg




LOCAL_SRC_FILES := GisLibrary.cpp \
				   map.cpp



include $(BUILD_SHARED_LIBRARY)

#ThirdParty
$(call import-add-path, $(FHIRD_PARTY_LIB_PATH))
$(call import-module, freetype)
$(call import-module, proj4)
$(call import-module, ShapeLib)
$(call import-module, zlib)
$(call import-module, png)
$(call import-module, jpeg)


$(call import-add-path, $(COMMON_LIB_PATH))
$(call import-module, CommonLibrary)

$(call import-add-path, $(EMBDB_LIB_PATH))
$(call import-module, EmbDB)


#GisEngine

$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)

