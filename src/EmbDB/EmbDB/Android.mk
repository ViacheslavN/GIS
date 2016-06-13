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
ThirdParty_I_PATH := ..\..\..\..\..\ThirdParty



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
		-I$(ThirdParty_I_PATH) \
		-x c++
#storage
LOCAL_SRC_FILES :=  FilePage.cpp storage.cpp StorageInfo.cpp
#Table
LOCAL_SRC_FILES +=Table.cpp   
#transactions
LOCAL_SRC_FILES +=Transactions.cpp TransactionCache.cpp TranStorage.cpp TranUndoPageManager.cpp  TranRedoPageManager.cpp  TranLogStateManager.cpp DirectTransactions.cpp TranPerfCounter.cpp
#Cursors
LOCAL_SRC_FILES +=InsertCursor.cpp  DeleteCursor.cpp  SimpleSearchCursor.cpp SimpleSelectCursor.cpp
#database
LOCAL_SRC_FILES +=Database.cpp DBStateManager.cpp DBTranManager.cpp Schema.cpp
#sdk
LOCAL_SRC_FILES +=Fields.cpp FieldSet.cpp Row.cpp
#utils
LOCAL_SRC_FILES += CRC.cpp MathUtils.cpp
#alloc
LOCAL_SRC_FILES += PageAlloc.cpp MemPageCache.cpp
#Z-Order
LOCAL_SRC_FILES += SpatialPointQuery.cpp  SpatialRectQuery.cpp
#compress
LOCAL_SRC_FILES += LinkCompress.cpp OIDCompress.cpp ZLibCompressor.cpp
#BPTRee
LOCAL_SRC_FILES += BPTreeStatistics.cpp


LOCAL_STATIC_LIBRARIES := libcommon

include $(BUILD_STATIC_LIBRARY)
