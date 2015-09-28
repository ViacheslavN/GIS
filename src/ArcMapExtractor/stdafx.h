// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define  _WTL_USE_CSTRING

#include "resource.h"
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include <atltypes.h>

#include "esriCore.h"
#include "ArcCATIDs.h"

#include <vector>


#include "CommonLibrary/CommonLib_lib.h"

#define COM_CHECK_PTR(ptr) if(ptr == 0) return E_POINTER;