/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/statline.cpp
// Purpose:     wxStaticLine class
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id: statline.cpp 64943 2010-07-13 13:29:58Z VZ $
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/statline.h"

#ifndef WX_PRECOMP
    #include "wx/statbox.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)


bool wxStaticLine::Create( wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    long style,
    const wxString &name )
{
    if ( !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    // this is ugly but it's better than nothing:
    // use a thin static box to emulate static line

    wxSize sizeReal = AdjustSize( size );

//    m_statbox = new wxStaticBox( parent, id, wxT(""), pos, sizeReal, style, name );

    return true;
}
