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

LOCAL_MODULE    := libcommon




STLPORT_BASE := c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport
#LOCAL_CPPFLAGS := -fexceptions
LOCAL_CFLAGS :=	-DANDROID \
		-DUNICODE_STRING_OWN_IMPL \
		-I. \
		-Istlport \
		-I$(STLPORT_BASE)/stlport \
		-x c++

LOCAL_SRC_FILES :=  Interlocked.cpp  FilePosix.cpp blob.cpp String.cpp sprintf.cpp crc.cpp caseutils.cpp BoundaryBox.cpp GeoShape.cpp \
					IGeoShape.cpp  BitStream.cpp FixedBitStream.cpp WriteBitStream.cpp  FileStream.cpp FixedMemoryStream.cpp MemoryStream.cpp stream.cpp \
					guit.cpp multibyte.cpp thread.cpp alloc_t.cpp  Variant.cpp

include $(BUILD_STATIC_LIBRARY)
