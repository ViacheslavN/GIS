LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := GisLibrary

STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
COMMON_LIB_PATH := ..\..\..\..\..\CommonLibrary

EMBDB_I_PATH := ..\..\..\..\..\EmbDB
EMBDB_LIB_PATH := ..\..\..\..\..\EmbDB


FHIRD_PARTY_LIB_PATH := ..\..\..\..\..\ThirdParty

FREETYPE_I_PATH := ..\..\..\..\..\ThirdParty\freetype\include
PROJ4_I_PATH := ..\..\..\..\..\ThirdParty\proj4\src
SHAPELIB_I_PATH := ..\..\..\..\..\ThirdParty\ShapeLib
ZLIB_I_PATH := ..\..\..\..\..\ThirdParty\zlib
PNG_I_PATH := ..\..\..\..\..\ThirdParty\PNG
JPEG_I_PATH := ..\..\..\..\..\ThirdParty\jpeg

GIS_ENGINE_LIB_PATH := ..\..\..\..\..\GisEngine

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
		-I$(GIS_ENGINE_LIB_PATH) \
		-x c++	\
		-fexceptions


LOCAL_STATIC_LIBRARIES := libcommon
#EmbDB
LOCAL_STATIC_LIBRARIES += libEmbDB libDatasetLite
#ThirdParty
LOCAL_STATIC_LIBRARIES += libfreetype libproj4 libshapelib libzlib libpng libjpeg
#GisEngine
LOCAL_STATIC_LIBRARIES += libagg libDisplay libGisCommon libCartography libGeomerty libGeoDatabase
LOCAL_LDLIBS := -llog



LOCAL_SRC_FILES := MapDrawer.cpp GisLibrary.cpp map.cpp 



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
$(call import-module, DatasetLite)

#GisEngine
$(call import-add-path, $(GIS_ENGINE_LIB_PATH))
$(call import-module, Common)
$(call import-module, agg)
$(call import-module, Display)
$(call import-module, Cartography)
$(call import-module, GisGeometry)
$(call import-module, GeoDatabase)


$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)

