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

LOCAL_MODULE := libagg



STLPORT_BASE := c:/eclipse/android-ndk-r5c/sources/cxx-stl/stlport
SOFTWARE_INCLUDE := D:/software/include
COMMON_INCLUDE_PATH := D:/work/UniGIS/trunk/UGLite/
FREETYPE_INCLUDE_PATH := D:/ThirdParty/freetypeandroid/include/
LOCAL_CPPFLAGS := -fexceptions

LOCAL_C_INCLUDES := $(SOFTWARE_INCLUDE)	\
					$(COMMON_INCLUDE_PATH) \
					$(FREETYPE_INCLUDE_PATH) \
					$(STLPORT_BASE)/stlport

					
					
LOCAL_CFLAGS := -DANDROID_NDK \
		-D__NEW__ \
		-DANDROID \
		-DOS_ANDROID \
        -D_M_ARM \
		-DUNICODE_STRING_OWN_IMPL \
		-DUGD_LITE \
		-DUSE_PTHREADS\
		-Istlport \
		-I$(STLPORT_BASE)/stlport \
		-x c++

LOCAL_SRC_FILES :=agg_arc.cpp \
agg_arrowhead.cpp \
agg_bezier_arc.cpp \
agg_bspline.cpp \
agg_curves.cpp \
agg_embedded_raster_fonts.cpp \
agg_font_freetype.cpp \
agg_gsv_text.cpp \
agg_image_filters.cpp \
agg_line_aa_basics.cpp \
agg_line_profile_aa.cpp \
agg_rounded_rect.cpp \
agg_sqrt_tables.cpp \
agg_trans_affine.cpp \
agg_trans_double_path.cpp \
agg_trans_single_path.cpp \
agg_trans_warp_magnifier.cpp \
agg_vcgen_bspline.cpp \
agg_vcgen_contour.cpp \
agg_vcgen_dash.cpp \
agg_vcgen_markers_term.cpp \
agg_vcgen_smooth_poly1.cpp \
agg_vcgen_stroke.cpp \
agg_vpgen_clip_polygon.cpp \
agg_vpgen_clip_polyline.cpp \
agg_vpgen_segmentator.cpp \
polygon_clipper.cpp

LOCAL_STATIC_LIBRARIES := libfreetype

include $(BUILD_STATIC_LIBRARY)
$(call import-module, freetypeandroid) 