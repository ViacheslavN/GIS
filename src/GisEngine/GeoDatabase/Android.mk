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

LOCAL_MODULE    := libGeoDatabase
STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
TR1_BASE :=c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\gnu-libstdc++\4.9\include
		   
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
GIS_ENGINE_PATH := ..\..\..\..\..\GisEngine
THIRD_I_PATH := ..\..\..\..\..\ThirdParty
LOCAL_C_INCLUDES := $(LOCAL_PATH)\
					$(STLPORT_BASE)/stlport \
					$(COMMON_I_PATH) \
					$(GIS_ENGINE_PATH) \
					$(THIRD_I_PATH) \
					$(TR1_BASE) 
		
LOCAL_CFLAGS := -DANDROID \
				-D__GXX_EXPERIMENTAL_CXX0X__ \
				-I$(TR1_BASE) 
LOCAL_CPPFLAGS  := -std=c++11 \
				 -I$(TR1_BASE)

#Source Files
LOCAL_SRC_FILES := Feature.cpp Field.cpp Fields.cpp FieldSet.cpp GeoDatabase.cpp GeoDatabaseUtils.cpp GeometryDef.cpp OIDSet.cpp QueryFilter.cpp WorkspaceBase.cpp LoaderWorkspase.cpp
#GeoDatabaseShape
LOCAL_SRC_FILES += ShapefileFeatureClass.cpp ShapefileRowCursor.cpp ShapefileUtils.cpp ShapefileWorkspace.cpp
#GeoDatabaseSQlite
LOCAL_SRC_FILES += sqlite3\sqlite3.c sqlite3\shell.c SQLiteUtils.cpp SQLiteFeatureClass.cpp SQLiteInsertCursor.cpp SQLiteRowCursor.cpp SQLiteTable.cpp SQLiteTransaction.cpp SQLiteWorkspace.cpp
#GeoDatabaseEmbDB
LOCAL_SRC_FILES += EmbDBFeatureClass.cpp EmbDBInsertCursor.cpp EmbDBRowCursor.cpp EmbDBTable.cpp EmbDBTransaction.cpp embDBUtils.cpp EmbDBWorkspace.cpp

				   
LOCAL_STATIC_LIBRARIES := libEmbDB libDatasetLite libshapelib

include $(BUILD_STATIC_LIBRARY)
