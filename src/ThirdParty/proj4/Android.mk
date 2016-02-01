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

LOCAL_MODULE    := libproj4



LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/proj4\
					$(LOCAL_PATH) \
					$(LOCAL_PATH)/src/

LOCAL_CFLAGS := -DANDROID  

#azimuthal
LOCAL_SRC_FILES := src/PJ_aeqd.c src/PJ_gnom.c src/PJ_laea.c src/PJ_mod_ster.c	src/PJ_nsper.c src/PJ_nzmg.c src/PJ_ortho.c src/PJ_stere.c src/PJ_sterea.c src/proj_rouss.c
#conic
LOCAL_SRC_FILES += src/PJ_aea.c src/PJ_bipc.c src/PJ_bonne.c src/PJ_eqdc.c src/PJ_imwp.c src/PJ_lcc.c src/PJ_lcca.c src/PJ_poly.c src/PJ_rpoly.c src/PJ_sconics.c
#cylinder
LOCAL_SRC_FILES += src/PJ_cass.c src/PJ_cc.c src/PJ_cea.c src/PJ_eqc.c src/PJ_gall.c src/PJ_geos.c src/PJ_gstmerc.c src/PJ_labrd.c src/PJ_lsat.c src/PJ_merc.c \
				   src/PJ_mill.c src/PJ_ocea.c src/PJ_omerc.c src/PJ_somerc.c src/PJ_tcc.c src/PJ_tcea.c src/PJ_tmerc.c src/proj_etmerc.c
#misc
LOCAL_SRC_FILES += src/PJ_airy.c src/PJ_aitoff.c src/PJ_august.c src/PJ_bacon.c src/PJ_chamb.c src/pj_geocent.c src/PJ_hammer.c src/PJ_healpix.c src/PJ_krovak.c \
				   src/PJ_lagrng.c src/PJ_larr.c src/PJ_lask.c src/pj_latlong.c src/PJ_natearth.c src/PJ_nocol.c src/PJ_ob_tran.c src/PJ_oea.c src/PJ_qsc.c src/PJ_tpeqd.c	\
				   src/PJ_vandg.c src/PJ_vandg2.c	 src/PJ_vandg4.c src/PJ_wag7.c
#pseudo
LOCAL_SRC_FILES += src/PJ_boggs.c src/PJ_calcofi.c src/PJ_collg.c src/PJ_crast.c src/PJ_denoy.c src/PJ_eck1.c src/PJ_eck2.c src/PJ_eck3.c src/PJ_eck4.c src/PJ_eck5.c src/PJ_fahey.c \
				   src/PJ_fouc_s.c src/PJ_gins8.c src/PJ_gn_sinu.c src/PJ_goode.c src/PJ_hatano.c src/PJ_igh.c src/PJ_isea.c src/PJ_loxim.c src/PJ_mbt_fps.c src/PJ_mbtfpp.c src/PJ_mbtfpq.c \
				   src/PJ_moll.c src/PJ_nell.c src/PJ_putp2.c src/PJ_putp3.c src/PJ_putp4p.c src/PJ_putp5.c src/PJ_putp6.c src/PJ_robin.c src/PJ_sts.c src/PJ_urm5.c src/PJ_urmfps.c src/PJ_wag2.c \
				   src/PJ_wag3.c src/PJ_wink1.c src/PJ_wink2.c
#support
LOCAL_SRC_FILES += src/aasincos.c src/adjlon.c src/bch2bps.c src/bchgen.c src/biveval.c src/dmstor.c src/geocent.c src/mk_cheby.c src/nad_cvt.c src/nad_init.c src/nad_intr.c src/pj_apply_gridshift.c \
				   src/pj_apply_vgridshift.c src/pj_auth.c src/pj_ctx.c src/pj_datum_set.c src/pj_datums.c src/pj_deriv.c src/pj_ell_set.c src/pj_ellps.c src/pj_errno.c src/pj_factors.c src/pj_fileapi.c \
				   src/pj_fwd.c src/pj_gauss.c src/pj_gc_reader.c src/pj_gridcatalog.c src/pj_gridinfo.c src/pj_gridlist.c src/pj_init.c src/pj_initcache.c src/pj_inv.c src/pj_list.c src/pj_log.c \
				   src/pj_malloc.c src/pj_mlfn.c src/pj_msfn.c src/pj_mutex.c src/pj_open_lib.c src/pj_param.c src/pj_phi2.c src/pj_pr_list.c src/pj_qsfn.c src/pj_release.c src/pj_strerrno.c \
				   src/pj_transform.c src/pj_tsfn.c src/pj_units.c src/pj_zpoly1.c src/proj_mdist.c src/rtodms.c src/vector1.c
include $(BUILD_STATIC_LIBRARY)
