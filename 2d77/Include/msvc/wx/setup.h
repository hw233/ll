/////////////////////////////////////////////////////////////////////////////
// Name:        msvc/wx/msw/setup.h
// Purpose:     wrapper around the real wx/setup.h for Visual C++
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-12-12
// RCS-ID:      $Id: setup.h 63920 2010-04-09 06:55:19Z VS $
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MSC_VER
    #error "This file should only be included when using Microsoft Visual C++"
#endif

// VC++ IDE predefines _DEBUG and _UNICODE for the new projects itself, but
// the other symbols (WXUSINGDLL, __WXUNIVERSAL__, ...) should be defined
// explicitly!

#include "wx/version.h"
#include "wx/cpp.h"

// notice that wxSUFFIX_DEBUG is a string but wxSUFFIX itself must be an
// identifier as string concatenation is not done inside #include where we
// need it
#ifdef _DEBUG
    #define wxSUFFIX_DEBUG "d"
    #ifdef _UNICODE
        #define wxSUFFIX ud
    #else // !_UNICODE
        #define wxSUFFIX d
    #endif // _UNICODE/!_UNICODE
#else
    #define wxSUFFIX_DEBUG ""
    #ifdef _UNICODE
        #define wxSUFFIX u
    #else // !_UNICODE
        #define wxSUFFIX
    #endif // _UNICODE/!_UNICODE
#endif

// compiler-specific prefix: by default it's always just "vc" for compatibility
// reasons but if you use multiple MSVC versions you probably build them with
// COMPILER_PREFIX=vcXX and in this case you may want to either predefine
// wxMSVC_VERSION as "XX" or define wxMSVC_VERSION_AUTO to use the appropriate
// version depending on the compiler used
#ifdef wxMSVC_VERSION
    #define wxCOMPILER_PREFIX wxCONCAT2(vc, wxMSVC_VERSION)
#elif defined(wxMSVC_VERSION_AUTO)
    #if _MSC_VER == 1200
        #define wxCOMPILER_PREFIX vc60
    #elif _MSC_VER == 1300
        #define wxCOMPILER_PREFIX vc70
    #elif _MSC_VER == 1310
        #define wxCOMPILER_PREFIX vc71
    #elif _MSC_VER == 1400
        #define wxCOMPILER_PREFIX vc80
    #elif _MSC_VER == 1500
        #define wxCOMPILER_PREFIX vc90
    #elif _MSC_VER == 1600
        #define wxCOMPILER_PREFIX vc100
    #else
        #error "Unknown MSVC compiler version, please report to wx-dev."
    #endif
#else
    #define wxCOMPILER_PREFIX vc
#endif

// architecture-specific part: not used (again, for compatibility), for x86
#if defined(_M_X64)
    #define wxARCH_SUFFIX _amd64
#elif defined(_M_IA64)
    #define wxARCH_SUFFIX _ia64
#else // assume _M_IX86
    #define wxARCH_SUFFIX
#endif

#ifdef WXUSINGDLL
    #define wxLIB_SUBDIR wxCONCAT3(wxCOMPILER_PREFIX, wxARCH_SUFFIX, _dll)
#else // !DLL
    #define wxLIB_SUBDIR wxCONCAT3(wxCOMPILER_PREFIX, wxARCH_SUFFIX, _lib)
#endif // DLL/!DLL


// the real setup.h header file we need is in the build-specific directory,
// construct the path to it
#define wxSETUPH_PATH \
    wxCONCAT4(../../../lib/, /msw, wxSUFFIX, /wx/setup.h)
#define wxSETUPH_PATH_STR wxSTRINGIZE(wxSETUPH_PATH)

#include wxSETUPH_PATH_STR


// the library names depend on the build, these macro builds the correct
// library name for the given base name
#define wxSUFFIX_STR wxSTRINGIZE(wxSUFFIX)
#define wxSHORT_VERSION_STRING \
    wxSTRINGIZE(wxMAJOR_VERSION) wxSTRINGIZE(wxMINOR_VERSION)

#define wxWX_LIB_NAME(name, subname) \
    "wx" name wxSHORT_VERSION_STRING wxSUFFIX_STR subname

#define wxBASE_LIB_NAME(name) wxWX_LIB_NAME("base", "_" name)
#define wxMSW_LIB_NAME(name) wxWX_LIB_NAME("msw", "_" name)

// this one is for 3rd party libraries: they don't have the version number
// in their names and usually exist in ANSI version only (except for regex)
#define wx3RD_PARTY_LIB_NAME(name) "wx" name wxSUFFIX_DEBUG

// special version for regex as it does have a Unicode version
#define wx3RD_PARTY_LIB_NAME_U(name) "wx" name wxSUFFIX_STR

#pragma comment(lib, wxWX_LIB_NAME("base", ""))

#ifndef wxNO_NET_LIB
    #pragma comment(lib, wxBASE_LIB_NAME("net"))
#endif
#ifndef wxNO_XML_LIB
    #pragma comment(lib, wxBASE_LIB_NAME("xml"))
#endif
#if wxUSE_REGEX && !defined(wxNO_REGEX_LIB)
    #pragma comment(lib, wx3RD_PARTY_LIB_NAME_U("regex"))
#endif

#if wxUSE_GUI
    #if wxUSE_XML && !defined(wxNO_EXPAT_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("expat"))
    #endif
    #if wxUSE_LIBJPEG && !defined(wxNO_JPEG_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("jpeg"))
    #endif
    #if wxUSE_LIBPNG && !defined(wxNO_PNG_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("png"))
    #endif
    #if wxUSE_LIBTIFF && !defined(wxNO_TIFF_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("tiff"))
    #endif
    #if wxUSE_ZLIB && !defined(wxNO_ZLIB_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("zlib"))
    #endif

    #pragma comment(lib, wxMSW_LIB_NAME("core"))

    #ifndef wxNO_ADV_LIB
        #pragma comment(lib, wxMSW_LIB_NAME("adv"))
    #endif

    #ifndef wxNO_HTML_LIB
        #pragma comment(lib, wxMSW_LIB_NAME("html"))
    #endif
    #if wxUSE_GLCANVAS && !defined(wxNO_GL_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("gl"))
    #endif
    #if wxUSE_DEBUGREPORT && !defined(wxNO_QA_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("qa"))
    #endif
    #if wxUSE_XRC && !defined(wxNO_XRC_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("xrc"))
    #endif
    #if wxUSE_AUI && !defined(wxNO_AUI_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("aui"))
    #endif
    #if wxUSE_PROPGRID && !defined(wxNO_PROPGRID_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("propgrid"))
    #endif
    #if wxUSE_RIBBON && !defined(wxNO_RIBBON_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("ribbon"))
    #endif
    #if wxUSE_RICHTEXT && !defined(wxNO_RICHTEXT_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("richtext"))
    #endif
    #if wxUSE_MEDIACTRL && !defined(wxNO_MEDIA_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("media"))
    #endif
    #if wxUSE_STC && !defined(wxNO_STC_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("stc"))
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("scintilla"))
    #endif
#endif // wxUSE_GUI


#ifndef WXUSINGDLL
    // Make sure all required system libraries are added to the linker too when
    // using static libraries.
    #pragma comment(lib, "kernel32")
    #pragma comment(lib, "user32")
    #pragma comment(lib, "gdi32")
    #pragma comment(lib, "comdlg32")
    #pragma comment(lib, "winspool")
    #pragma comment(lib, "winmm")
    #pragma comment(lib, "shell32")
    #pragma comment(lib, "comctl32")
    #pragma comment(lib, "ole32")
    #pragma comment(lib, "oleaut32")
    #pragma comment(lib, "uuid")
    #pragma comment(lib, "rpcrt4")
    #pragma comment(lib, "advapi32")
    #pragma comment(lib, "wsock32")
    #if wxUSE_URL_NATIVE
        #pragma comment(lib, "wininet")
    #endif
#endif // !WXUSINGDLL
