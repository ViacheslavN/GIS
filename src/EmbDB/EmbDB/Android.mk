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

LOCAL_MODULE    := libEmbDB

COMMON_I_PATH := ..\..\..\..\..\CommonLibrary




STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
#LOCAL_CPPFLAGS := -fexceptions
APP_CFLAGS += -std=c++11
APP_CPPFLAGS += -std=c++11
LOCAL_CFLAGS :=	-DANDROID \
		-DUNICODE_STRING_OWN_IMPL \
		-I. \
		-Istlport \
		-I$(STLPORT_BASE)/stlport \
		-I$(COMMON_I_PATH) \
		-x c++
#storage
LOCAL_SRC_FILES :=  FilePage.cpp storage.cpp StorageInfo.cpp
#Table
LOCAL_SRC_FILES +=Table.cpp   

include $(BUILD_STATIC_LIBRARY)
