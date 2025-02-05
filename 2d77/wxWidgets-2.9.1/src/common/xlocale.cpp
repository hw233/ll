//////////////////////////////////////////////////////////////////////////////
// Name:        src/common/xlocale.cpp
// Purpose:     xlocale wrappers/impl to provide some xlocale wrappers
// Author:      Brian Vanderburg II, Vadim Zeitlin
// Created:     2008-01-07
// RCS-ID:      $Id: xlocale.cpp 63574 2010-02-28 11:08:38Z VZ $
// Copyright:   (c) 2008 Brian Vanderburg II
//                  2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XLOCALE

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif

#include "wx/xlocale.h"

#include <errno.h>
#include <locale.h>

// ----------------------------------------------------------------------------
// module globals
// ----------------------------------------------------------------------------

// This is the C locale object, it is created on demand
static wxXLocale *gs_cLocale = NULL;

wxXLocale wxNullXLocale;


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Module for gs_cLocale cleanup
// ----------------------------------------------------------------------------

class wxXLocaleModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDELETE(gs_cLocale); }

    DECLARE_DYNAMIC_CLASS(wxXLocaleModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxXLocaleModule, wxModule)


// ============================================================================
// wxXLocale implementation
// ============================================================================

// ----------------------------------------------------------------------------
// common parts
// ----------------------------------------------------------------------------

// Get the C locale
wxXLocale& wxXLocale::GetCLocale()
{
    if ( !gs_cLocale )
    {
        gs_cLocale = new wxXLocale(static_cast<wxXLocaleCTag *>(NULL));
    }

    return *gs_cLocale;
}

#ifdef wxHAS_XLOCALE_SUPPORT

wxXLocale::wxXLocale(wxLanguage lang)
{
    const wxLanguageInfo * const info = wxLocale::GetLanguageInfo(lang);
    if ( !info )
    {
        m_locale = NULL;
    }
    else
    {
        Init(info->GetLocaleName().c_str());
    }
}

#if wxCHECK_VISUALC_VERSION(8)

// ----------------------------------------------------------------------------
// implementation using MSVC locale API
// ----------------------------------------------------------------------------

void wxXLocale::Init(const char *loc)
{
    if (!loc || *loc == '\0')
        return;

    m_locale = _create_locale(LC_ALL, loc);
}

void wxXLocale::Free()
{
    if ( m_locale )
        _free_locale(m_locale);
}

#elif defined(HAVE_LOCALE_T)

// ----------------------------------------------------------------------------
// implementation using xlocale API
// ----------------------------------------------------------------------------

void wxXLocale::Init(const char *loc)
{
    if (!loc || *loc == '\0')
        return;

    m_locale = newlocale(LC_ALL_MASK, loc, NULL);
    if (!m_locale)
    {
        // NOTE: here we do something similar to what wxSetLocaleTryUTF8() does
        //       in wxLocale code (but with newlocale() calls instead of wxSetlocale())
        wxString buf(loc);
        wxString buf2;
        buf2 = buf + wxS(".UTF-8");
        m_locale = newlocale(LC_ALL_MASK, buf2.c_str(), NULL);
        if ( !m_locale )
        {
            buf2 = buf + wxS(".utf-8");
            m_locale = newlocale(LC_ALL_MASK, buf2.c_str(), NULL);
        }
        if ( !m_locale )
        {
            buf2 = buf + wxS(".UTF8");
            m_locale = newlocale(LC_ALL_MASK, buf2.c_str(), NULL);
        }
        if ( !m_locale )
        {
            buf2 = buf + wxS(".utf8");
            m_locale = newlocale(LC_ALL_MASK, buf2.c_str(), NULL);
        }
    }

    // TODO: wxLocale performs many more manipulations of the given locale
    //       string in the attempt to set a valid locale; reusing that code
    //       (changing it to take a generic wxTryLocale callback) would be nice
}

void wxXLocale::Free()
{
    if ( m_locale )
        freelocale(m_locale);
}

#else
    #error "Unknown xlocale support."
#endif

#endif // wxHAS_XLOCALE_SUPPORT

#ifndef wxHAS_XLOCALE_SUPPORT

// ============================================================================
// Implementation of wxFoo_l() functions for "C" locale without xlocale support
// ============================================================================

// ----------------------------------------------------------------------------
// character classification and transformation functions
// ----------------------------------------------------------------------------

// lookup table and macros for character type functions
#define CTYPE_ALNUM 0x0001
#define CTYPE_ALPHA 0x0002
#define CTYPE_CNTRL 0x0004
#define CTYPE_DIGIT 0x0008
#define CTYPE_GRAPH 0x0010
#define CTYPE_LOWER 0x0020
#define CTYPE_PRINT 0x0040
#define CTYPE_PUNCT 0x0080
#define CTYPE_SPACE 0x0100
#define CTYPE_UPPER 0x0200
#define CTYPE_XDIGIT 0x0400

static const unsigned int gs_lookup[] =
{
    0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0004, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0004, 0x0004,
    0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0140, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0,
    0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0,
    0x0459, 0x0459, 0x0459, 0x0459, 0x0459, 0x0459, 0x0459, 0x0459,
    0x0459, 0x0459, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0,
    0x00D0, 0x0653, 0x0653, 0x0653, 0x0653, 0x0653, 0x0653, 0x0253,
    0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253,
    0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253, 0x0253,
    0x0253, 0x0253, 0x0253, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x00D0,
    0x00D0, 0x0473, 0x0473, 0x0473, 0x0473, 0x0473, 0x0473, 0x0073,
    0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073,
    0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073, 0x0073,
    0x0073, 0x0073, 0x0073, 0x00D0, 0x00D0, 0x00D0, 0x00D0, 0x0004
};


#define CTYPE_TEST(c, t) ( (c) <= 127 && (gs_lookup[(c)] & (t)) )


// ctype functions
#define GEN_ISFUNC(name, test) \
int name(const wxUniChar& c, const wxXLocale& loc) \
{ \
    wxCHECK(loc.IsOk(), false); \
    return CTYPE_TEST(c.GetValue(), test); \
}

GEN_ISFUNC(wxIsalnum_l, CTYPE_ALNUM)
GEN_ISFUNC(wxIsalpha_l, CTYPE_ALPHA)
GEN_ISFUNC(wxIscntrl_l, CTYPE_CNTRL)
GEN_ISFUNC(wxIsdigit_l, CTYPE_DIGIT)
GEN_ISFUNC(wxIsgraph_l, CTYPE_GRAPH)
GEN_ISFUNC(wxIslower_l, CTYPE_LOWER)
GEN_ISFUNC(wxIsprint_l, CTYPE_PRINT)
GEN_ISFUNC(wxIspunct_l, CTYPE_PUNCT)
GEN_ISFUNC(wxIsspace_l, CTYPE_SPACE)
GEN_ISFUNC(wxIsupper_l, CTYPE_UPPER)
GEN_ISFUNC(wxIsxdigit_l, CTYPE_XDIGIT)

int wxTolower_l(const wxUniChar& c, const wxXLocale& loc)
{
    wxCHECK(loc.IsOk(), false);

    if(CTYPE_TEST(c.GetValue(), CTYPE_UPPER))
    {
        return c - 'A' + 'a';
    }

    return c;
}

int wxToupper_l(const wxUniChar& c, const wxXLocale& loc)
{
    wxCHECK(loc.IsOk(), false);

    if(CTYPE_TEST(c.GetValue(), CTYPE_LOWER))
    {
        return c - 'a' + 'A';
    }

    return c;
}


// ----------------------------------------------------------------------------
// string --> number conversion functions
// ----------------------------------------------------------------------------

/*
    WARNING: the implementation of the wxStrtoX_l() functions below is unsafe
             in a multi-threaded environment as we temporary change the locale
             and if in the meanwhile an other thread performs some locale-dependent
             operation, it may get unexpected results...
             However this is the best we can do without reinventing the wheel in the
             case !wxHAS_XLOCALE_SUPPORT...
*/

/*
    Note that this code is similar to (a portion of) wxLocale::IsAvailable code
*/
#define IMPLEMENT_STRTOX_L_START                                \
    wxCHECK(loc.IsOk(), 0);                                     \
                                                                \
    /* (Try to) temporary set the 'C' locale */                 \
    const char *oldLocale = wxSetlocale(LC_NUMERIC, "C");       \
    if ( !oldLocale )                                           \
    {                                                           \
        /* the current locale was not changed; no need to */    \
        /* restore the previous one... */                       \
        errno = EINVAL;                                         \
            /* signal an error (better than nothing) */         \
        return 0;                                               \
    }

#define IMPLEMENT_STRTOX_L_END                                  \
    /* restore the original locale */                           \
    wxSetlocale(LC_NUMERIC, oldLocale);                         \
    return ret;

double wxStrtod_l(const wchar_t* str, wchar_t **endptr, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    double ret = wxStrtod(str, endptr);
    IMPLEMENT_STRTOX_L_END
}

double wxStrtod_l(const char* str, char **endptr, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    double ret = wxStrtod(str, endptr);
    IMPLEMENT_STRTOX_L_END
}

long wxStrtol_l(const wchar_t* str, wchar_t **endptr, int base, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    long ret = wxStrtol(str, endptr, base);
    IMPLEMENT_STRTOX_L_END
}

long wxStrtol_l(const char* str, char **endptr, int base, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    long ret = wxStrtol(str, endptr, base);
    IMPLEMENT_STRTOX_L_END
}

unsigned long wxStrtoul_l(const wchar_t* str, wchar_t **endptr, int base, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    unsigned long ret = wxStrtoul(str, endptr, base);
    IMPLEMENT_STRTOX_L_END
}

unsigned long wxStrtoul_l(const char* str, char **endptr, int base, const wxXLocale& loc)
{
    IMPLEMENT_STRTOX_L_START
    unsigned long ret = wxStrtoul(str, endptr, base);
    IMPLEMENT_STRTOX_L_END
}


#endif // !defined(wxHAS_XLOCALE_SUPPORT)

#endif // wxUSE_XLOCALE
