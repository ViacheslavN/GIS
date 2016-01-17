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

LOCAL_MODULE    := libGeomerty
STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
COMMON_I_PATH := ..\..\..\..\..\CommonLibrary
GIS_ENGINE_PATH := ..\..\..\..\..\GisEngine
PROJ4_I_PATH := ..\..\..\..\..\ThirdParty\proj4
PROJ4_I_PATH := ..\..\..\..\..\ThirdParty\proj4
LOCAL_C_INCLUDES := $(LOCAL_PATH)\
					$(STLPORT_BASE)/stlport \
					$(COMMON_I_PATH) \
					$(GIS_ENGINE_PATH) \
					$(PROJ4_I_PATH) \
					$(LOCAL_PATH)/cpl \
					$(LOCAL_PATH)/ogr					
LOCAL_CFLAGS := -DANDROID \
				-x c++  \
				-Dunix \
				-fexceptions

#Source Files
LOCAL_SRC_FILES := Envelope.cpp
#SpatialReferenceProj4
LOCAL_SRC_FILES += CodeProj.cpp SpatialReferenceProj4.cpp
#TopologicalOperator
LOCAL_SRC_FILES += clipper.cpp TopologicalOperator.cpp
#cpl
LOCAL_SRC_FILES += cpl\cpl_atomic_ops.cpp cpl\cpl_conv.cpp cpl\cpl_csv.cpp cpl\cpl_error.cpp cpl\cpl_findfile.cpp cpl\cpl_http.cpp cpl\cpl_minixml.cpp cpl\cpl_multiproc.cpp \
				   cpl\cpl_path.cpp cpl\cpl_recode.cpp cpl\cpl_recode_stub.cpp cpl\cpl_string.cpp cpl\cpl_strtod.cpp cpl\cpl_vsi_mem.cpp cpl\cpl_vsil.cpp cpl\cpl_vsil_abstract_archive.cpp \
				   cpl\cpl_vsil_cache.cpp cpl\cpl_vsil_sparsefile.cpp cpl\cpl_vsil_stdin.cpp cpl\cpl_vsil_stdout.cpp cpl\cpl_vsil_subfile.cpp cpl\cpl_vsil_tar.cpp cpl\cpl_vsisimple.cpp cpl\cplgetsymbol.cpp \
				   cpl\cplstring.cpp cpl\cplstringlist.cpp cpl\cpl_vsil_unix_stdio_64.cpp
#ogr
LOCAL_SRC_FILES += ogr\ogr_fromepsg.cpp ogr\ogr_srs_dict.cpp ogr\ogr_srs_esri.cpp ogr\ogr_srs_proj4.cpp ogr\ogr_srs_xml.cpp ogr\ogr_srsnode.cpp	ogr\ogrct.cpp ogr\ogrspatialreference.cpp			   
				   

include $(BUILD_STATIC_LIBRARY)
