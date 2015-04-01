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

LOCAL_MODULE    := libjpeg-6b



LOCAL_C_INCLUDES := $(LOCAL_PATH)
					

LOCAL_CFLAGS := -DANDROID_NDK \
		-D__NEW__ \
		-DANDROID \
		-DOS_ANDROID \
		-DXMD_H \
        -D_M_ARM \
		-DUNICODE_STRING_OWN_IMPL \
		-fvisibility=hidden

LOCAL_SRC_FILES := 	Jcapimin.c \
	Jcapistd.c \
	Jccoefct.c \
	Jccolor.c \
	Jcdctmgr.c \
	Jchuff.c \
	Jcinit.c \
	Jcmainct.c \
	Jcmarker.c \
	Jcmaster.c \
	Jcomapi.c \
	Jcparam.c \
	Jcphuff.c \
	Jcprepct.c \
	Jcsample.c \
	Jctrans.c \
	Jdapimin.c \
	Jdapistd.c \
	Jdatadst.c \
	Jdatasrc.c \
	Jdcoefct.c \
	Jdcolor.c \
	Jddctmgr.c \
	Jdhuff.c \
	Jdinput.c \
	Jdmainct.c \
	Jdmarker.c \
	Jdmaster.c \
	Jdmerge.c \
	Jdphuff.c \
	Jdpostct.c \
	Jdsample.c \
	Jdtrans.c \
	Jerror.c \
	Jfdctflt.c \
	Jfdctfst.c \
	Jfdctint.c \
	Jidctflt.c \
	Jidctfst.c \
	Jidctint.c \
	Jidctred.c \
	Jmemmgr.c \
	Jmemnobs.c \
	jmemsrc.c \
	Jquant1.c \
	Jquant2.c \
	Jutils.c




include $(BUILD_STATIC_LIBRARY)
