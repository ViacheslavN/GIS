

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Sep 12 17:30:18 2015
 */
/* Compiler settings for ArcMapExtractor.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ArcMapExtractor_i_h__
#define __ArcMapExtractor_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMapToolBar_FWD_DEFINED__
#define __IMapToolBar_FWD_DEFINED__
typedef interface IMapToolBar IMapToolBar;
#endif 	/* __IMapToolBar_FWD_DEFINED__ */


#ifndef __IExtractCommand_FWD_DEFINED__
#define __IExtractCommand_FWD_DEFINED__
typedef interface IExtractCommand IExtractCommand;
#endif 	/* __IExtractCommand_FWD_DEFINED__ */


#ifndef __MapToolBar_FWD_DEFINED__
#define __MapToolBar_FWD_DEFINED__

#ifdef __cplusplus
typedef class MapToolBar MapToolBar;
#else
typedef struct MapToolBar MapToolBar;
#endif /* __cplusplus */

#endif 	/* __MapToolBar_FWD_DEFINED__ */


#ifndef __ExtractCommand_FWD_DEFINED__
#define __ExtractCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class ExtractCommand ExtractCommand;
#else
typedef struct ExtractCommand ExtractCommand;
#endif /* __cplusplus */

#endif 	/* __ExtractCommand_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMapToolBar_INTERFACE_DEFINED__
#define __IMapToolBar_INTERFACE_DEFINED__

/* interface IMapToolBar */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMapToolBar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932CE063-F48F-4108-B19C-E2FE50430E40")
    IMapToolBar : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMapToolBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMapToolBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMapToolBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMapToolBar * This);
        
        END_INTERFACE
    } IMapToolBarVtbl;

    interface IMapToolBar
    {
        CONST_VTBL struct IMapToolBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMapToolBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMapToolBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMapToolBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMapToolBar_INTERFACE_DEFINED__ */


#ifndef __IExtractCommand_INTERFACE_DEFINED__
#define __IExtractCommand_INTERFACE_DEFINED__

/* interface IExtractCommand */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IExtractCommand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("44F5D25D-9773-490C-B0B6-2F61570A0867")
    IExtractCommand : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IExtractCommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExtractCommand * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExtractCommand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExtractCommand * This);
        
        END_INTERFACE
    } IExtractCommandVtbl;

    interface IExtractCommand
    {
        CONST_VTBL struct IExtractCommandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExtractCommand_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IExtractCommand_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IExtractCommand_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IExtractCommand_INTERFACE_DEFINED__ */



#ifndef __ArcMapExtractorLib_LIBRARY_DEFINED__
#define __ArcMapExtractorLib_LIBRARY_DEFINED__

/* library ArcMapExtractorLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_ArcMapExtractorLib;

EXTERN_C const CLSID CLSID_MapToolBar;

#ifdef __cplusplus

class DECLSPEC_UUID("EC8D7051-06F0-46E1-8BC1-167B4A0FACB3")
MapToolBar;
#endif

EXTERN_C const CLSID CLSID_ExtractCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("444FF0D6-E29E-4402-80C6-7252FEC7FA03")
ExtractCommand;
#endif
#endif /* __ArcMapExtractorLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


