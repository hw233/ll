/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dlmsw.cpp
// Purpose:     Win32-specific part of wxDynamicLibrary and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-10 (partly extracted from common/dynlib.cpp)
// RCS-ID:      $Id: dlmsw.cpp 64139 2010-04-25 12:07:37Z VS $
// Copyright:   (c) 1998-2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_DYNLIB_CLASS

#include "wx/msw/private.h"
#include "wx/msw/debughlp.h"

const wxString wxDynamicLibrary::ms_dllext(wxT(".dll"));

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// wrap some functions from version.dll: load them dynamically and provide a
// clean interface
class wxVersionDLL
{
public:
    // load version.dll and bind to its functions
    wxVersionDLL();

    // return the file version as string, e.g. "x.y.z.w"
    wxString GetFileVersion(const wxString& filename) const;

private:
    typedef DWORD (APIENTRY *GetFileVersionInfoSize_t)(PTSTR, PDWORD);
    typedef BOOL (APIENTRY *GetFileVersionInfo_t)(PTSTR, DWORD, DWORD, PVOID);
    typedef BOOL (APIENTRY *VerQueryValue_t)(const PVOID, PTSTR, PVOID *, PUINT);

    #define DO_FOR_ALL_VER_FUNCS(what)                                        \
        what(GetFileVersionInfoSize);                                         \
        what(GetFileVersionInfo);                                             \
        what(VerQueryValue)

    #define DECLARE_VER_FUNCTION(func) func ## _t m_pfn ## func

    DO_FOR_ALL_VER_FUNCS(DECLARE_VER_FUNCTION);

    #undef DECLARE_VER_FUNCTION


    wxDynamicLibrary m_dll;


    wxDECLARE_NO_COPY_CLASS(wxVersionDLL);
};

// class used to create wxDynamicLibraryDetails objects
class WXDLLIMPEXP_BASE wxDynamicLibraryDetailsCreator
{
public:
    // type of parameters being passed to EnumModulesProc
    struct EnumModulesProcParams
    {
        wxDynamicLibraryDetailsArray *dlls;
        wxVersionDLL *verDLL;
    };

    // the declared type of the first EnumModulesProc() parameter changed in
    // recent SDK versions and is no PCSTR instead of old PSTR, we know that
    // it's const in version 11 and non-const in version 8 included with VC8
    // (and earlier), suppose that it's only changed in version 11
    #if defined(API_VERSION_NUMBER) && API_VERSION_NUMBER >= 11
        typedef PCSTR NameStr_t;
    #else
        typedef PSTR NameStr_t;
    #endif

    // TODO: fix EnumerateLoadedModules() to use EnumerateLoadedModules64()
    #ifdef __WIN64__
        typedef DWORD64 DWORD_32_64;
    #else
        typedef DWORD DWORD_32_64;
    #endif

    static BOOL CALLBACK
    EnumModulesProc(NameStr_t name, DWORD_32_64 base, ULONG size, void *data);
};

// ============================================================================
// wxVersionDLL implementation
// ============================================================================

// ----------------------------------------------------------------------------
// loading
// ----------------------------------------------------------------------------

wxVersionDLL::wxVersionDLL()
{
    // don't give errors if DLL can't be loaded or used, we're prepared to
    // handle it
    wxLogNull noLog;

    if ( m_dll.Load(wxT("version.dll"), wxDL_VERBATIM) )
    {
        // the functions we load have either 'A' or 'W' suffix depending on
        // whether we're in ANSI or Unicode build
        #ifdef UNICODE
            #define SUFFIX L"W"
        #else // ANSI
            #define SUFFIX "A"
        #endif // UNICODE/ANSI

        #define LOAD_VER_FUNCTION(name)                                       \
            m_pfn ## name = (name ## _t)m_dll.GetSymbol(wxT(#name SUFFIX));    \
        if ( !m_pfn ## name )                                                 \
        {                                                                     \
            m_dll.Unload();                                                   \
            return;                                                           \
        }

        DO_FOR_ALL_VER_FUNCS(LOAD_VER_FUNCTION);

        #undef LOAD_VER_FUNCTION
    }
}

// ----------------------------------------------------------------------------
// wxVersionDLL operations
// ----------------------------------------------------------------------------

wxString wxVersionDLL::GetFileVersion(const wxString& filename) const
{
    wxString ver;
    if ( m_dll.IsLoaded() )
    {
        wxChar *pc = const_cast<wxChar *>((const wxChar*) filename.t_str());

        DWORD dummy;
        DWORD sizeVerInfo = m_pfnGetFileVersionInfoSize(pc, &dummy);
        if ( sizeVerInfo )
        {
            wxCharBuffer buf(sizeVerInfo);
            if ( m_pfnGetFileVersionInfo(pc, 0, sizeVerInfo, buf.data()) )
            {
                void *pVer;
                UINT sizeInfo;
                if ( m_pfnVerQueryValue(buf.data(),
                                        const_cast<wxChar *>(wxT("\\")),
                                        &pVer,
                                        &sizeInfo) )
                {
                    VS_FIXEDFILEINFO *info = (VS_FIXEDFILEINFO *)pVer;
                    ver.Printf(wxT("%d.%d.%d.%d"),
                               HIWORD(info->dwFileVersionMS),
                               LOWORD(info->dwFileVersionMS),
                               HIWORD(info->dwFileVersionLS),
                               LOWORD(info->dwFileVersionLS));
                }
            }
        }
    }
    //else: we failed to load DLL, can't retrieve version info

    return ver;
}

// ============================================================================
// wxDynamicLibraryDetailsCreator implementation
// ============================================================================

/* static */
BOOL CALLBACK
wxDynamicLibraryDetailsCreator::EnumModulesProc(NameStr_t name,
                                                DWORD_32_64 base,
                                                ULONG size,
                                                void *data)
{
    EnumModulesProcParams *params = (EnumModulesProcParams *)data;

    wxDynamicLibraryDetails *details = new wxDynamicLibraryDetails;

    // fill in simple properties
    details->m_name = wxString::FromAscii(name);
    details->m_address = wxUIntToPtr(base);
    details->m_length = size;

    // to get the version, we first need the full path
    const HMODULE
        hmod = wxDynamicLibrary::MSWGetModuleHandle(name, details->m_address);
    if ( hmod )
    {
        wxString fullname = wxGetFullModuleName(hmod);
        if ( !fullname.empty() )
        {
            details->m_path = fullname;
            details->m_version = params->verDLL->GetFileVersion(fullname);
        }
    }

    params->dlls->Add(details);

    // continue enumeration (returning FALSE would have stopped it)
    return TRUE;
}

// ============================================================================
// wxDynamicLibrary implementation
// ============================================================================

// ----------------------------------------------------------------------------
// misc functions
// ----------------------------------------------------------------------------

wxDllType wxDynamicLibrary::GetProgramHandle()
{
    return (wxDllType)::GetModuleHandle(NULL);
}

// ----------------------------------------------------------------------------
// loading/unloading DLLs
// ----------------------------------------------------------------------------

/* static */
wxDllType
wxDynamicLibrary::RawLoad(const wxString& libname, int flags)
{
    return flags & wxDL_GET_LOADED
            ? ::GetModuleHandle(libname.t_str())
            : ::LoadLibrary(libname.t_str());
}

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
    ::FreeLibrary(handle);
}

/* static */
void *wxDynamicLibrary::RawGetSymbol(wxDllType handle, const wxString& name)
{
    return (void *)::GetProcAddress(handle,
#ifdef __WXWINCE__
                                            name.c_str()
#else
                                            name.ToAscii()
#endif // __WXWINCE__
                                   );
}

// ----------------------------------------------------------------------------
// enumerating loaded DLLs
// ----------------------------------------------------------------------------

/* static */
wxDynamicLibraryDetailsArray wxDynamicLibrary::ListLoaded()
{
    wxDynamicLibraryDetailsArray dlls;

#if wxUSE_DBGHELP
    if ( wxDbgHelpDLL::Init() )
    {
        // prepare to use functions for version info extraction
        wxVersionDLL verDLL;

        wxDynamicLibraryDetailsCreator::EnumModulesProcParams params;
        params.dlls = &dlls;
        params.verDLL = &verDLL;

        if ( !wxDbgHelpDLL::EnumerateLoadedModules
                            (
                                ::GetCurrentProcess(),
                                wxDynamicLibraryDetailsCreator::EnumModulesProc,
                                &params
                            ) )
        {
            wxLogLastError(wxT("EnumerateLoadedModules"));
        }
    }
#endif // wxUSE_DBGHELP

    return dlls;
}

/* static */
WXHMODULE wxDynamicLibrary::MSWGetModuleHandle(const char *name, void *addr)
{
    // we want to use GetModuleHandleEx() instead of usual GetModuleHandle()
    // because the former works correctly for comctl32.dll while the latter
    // returns NULL when comctl32.dll version 6 is used under XP (note that
    // GetModuleHandleEx() is only available under XP and later, coincidence?)

    // check if we can use GetModuleHandleEx
    typedef BOOL (WINAPI *GetModuleHandleEx_t)(DWORD, LPCSTR, HMODULE *);

    static const GetModuleHandleEx_t INVALID_FUNC_PTR = (GetModuleHandleEx_t)-1;

    static GetModuleHandleEx_t s_pfnGetModuleHandleEx = INVALID_FUNC_PTR;
    if ( s_pfnGetModuleHandleEx == INVALID_FUNC_PTR )
    {
        wxDynamicLibrary dll(wxT("kernel32.dll"), wxDL_VERBATIM);
        s_pfnGetModuleHandleEx =
            (GetModuleHandleEx_t)dll.RawGetSymbol(wxT("GetModuleHandleExA"));

        // dll object can be destroyed, kernel32.dll won't be unloaded anyhow
    }

    // get module handle from its address
    if ( s_pfnGetModuleHandleEx )
    {
        // flags are GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        //           GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
        HMODULE hmod;
        if ( s_pfnGetModuleHandleEx(6, (char *)addr, &hmod) && hmod )
            return hmod;
    }

    // Windows CE only has Unicode API, so even we have an ANSI string here, we
    // still need to use GetModuleHandleW() there
#ifdef __WXWINCE__
    return ::GetModuleHandleW(wxConvLibc.cMB2WC(name).data());
#else
    return ::GetModuleHandleA((char *)name);
#endif
}

#endif // wxUSE_DYNLIB_CLASS

