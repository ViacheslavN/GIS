# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE    := libGisCommon
STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
GIS_ENGINE_PATH := ..\..\..\..\..\GisEngine
FREETYPE_I_PATH := ..\..\..\..\..\ThirdParty\freetype\include
PNG_I_PATH := ..\..\..\..\..\ThirdParty\PNG
JPEG_I_PATH := ..\..\..\..\..\ThirdParty\jpeg
ZLIB_I_PATH := ..\..\..\..\..\ThirdParty\zlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)\
					$(STLPORT_BASE)/stlport \
					$(COMMON_I_PATH) \
					$(GIS_ENGINE_PATH) \
					$(FREETYPE_I_PATH) \
					$(PNG_I_PATH) \
					$(JPEG_I_PATH) \
					$(ZLIB_I_PATH) 
LOCAL_CFLAGS := -DANDROID \
				-x c++  

#src
LOCAL_SRC_FILES := PropertySet.cpp  TrackCancel.cpp Units.cpp
#xml
LOCAL_SRC_FILES += XMLDoc.cpp  XMLNode.cpp XMLUtils.cpp
 

include $(BUILD_STATIC_LIBRARY)
