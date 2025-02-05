/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/icon.cpp
// Purpose:     wxIcon class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: icon.cpp 64671 2010-06-21 16:36:45Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxOSX_USE_COCOA_OR_CARBON

#include "wx/icon.h"

#ifndef WX_PRECOMP
    #include "wx/image.h"
#endif

#include "wx/osx/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxGDIObject)

#define M_ICONDATA ((wxIconRefData *)m_refData)

class WXDLLEXPORT wxIconRefData : public wxGDIRefData
{
public:
    wxIconRefData() { Init(); }
    wxIconRefData( WXHICON iconref, int desiredWidth, int desiredHeight );
    virtual ~wxIconRefData() { Free(); }

    virtual bool IsOk() const { return m_iconRef != NULL; }

    virtual void Free();

    void SetWidth( int width ) { m_width = width; }
    void SetHeight( int height ) { m_height = height; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    WXHICON GetHICON() const { return (WXHICON) m_iconRef; }

private:
    void Init();

    IconRef m_iconRef;
    int m_width;
    int m_height;
};


wxIconRefData::wxIconRefData( WXHICON icon, int desiredWidth, int desiredHeight )
{
    m_iconRef = (IconRef)( icon ) ;

    // Standard sizes
    SetWidth( desiredWidth == -1 ? 32 : desiredWidth ) ;
    SetHeight( desiredHeight == -1 ? 32 : desiredHeight ) ;
}

void wxIconRefData::Init()
{
    m_iconRef = NULL ;
    m_width =
    m_height = 0;
}

void wxIconRefData::Free()
{
    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = NULL ;
    }
}

//
//
//

wxIcon::wxIcon()
{
}

wxIcon::wxIcon( const char bits[], int width, int height )
{
    wxBitmap bmp( bits, width, height ) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon(const char* const* bits)
{
    wxBitmap bmp( bits ) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon(
    const wxString& icon_file, wxBitmapType flags,
    int desiredWidth, int desiredHeight )
{
    LoadFile( icon_file, flags, desiredWidth, desiredHeight );
}

wxIcon::wxIcon(WXHICON icon, const wxSize& size)
      : wxGDIObject()
{
    // as the icon owns that ref, we have to acquire it as well
    if (icon)
        AcquireIconRef( (IconRef) icon ) ;

    m_refData = new wxIconRefData( icon, size.x, size.y ) ;
}

wxIcon::~wxIcon()
{
}

wxGDIRefData *wxIcon::CreateGDIRefData() const
{
    return new wxIconRefData;
}

wxGDIRefData *wxIcon::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxIconRefData(*static_cast<const wxIconRefData *>(data));
}

WXHICON wxIcon::GetHICON() const
{
    wxASSERT( Ok() ) ;

    return (WXHICON) ((wxIconRefData*)m_refData)->GetHICON() ;
}

int wxIcon::GetWidth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid icon") );

   return M_ICONDATA->GetWidth();
}

int wxIcon::GetHeight() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid icon") );

   return M_ICONDATA->GetHeight();
}

int wxIcon::GetDepth() const
{
    return 32;
}

void wxIcon::SetDepth( int WXUNUSED(depth) )
{
}

void wxIcon::SetWidth( int WXUNUSED(width) )
{
}

void wxIcon::SetHeight( int WXUNUSED(height) )
{
}

// Load an icon based on resource name or filel name
// Return true on success, false otherwise
bool wxIcon::LoadFile(
    const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight )
{
    if( type == wxBITMAP_TYPE_ICON_RESOURCE )
    {
        if( LoadIconFromSystemResource( filename, desiredWidth, desiredHeight ) )
            return true;
        else
            return LoadIconFromBundleResource( filename, desiredWidth, desiredHeight );
    }
    else  if( type == wxBITMAP_TYPE_ICON )
    {
        return LoadIconFromFile( filename, desiredWidth, desiredHeight );
    }
    else
    {
        return LoadIconAsBitmap( filename, type, desiredWidth, desiredHeight );
    }
}

// Load a well known system icon by its wxWidgets identifier
// Returns true on success, false otherwise
bool wxIcon::LoadIconFromSystemResource(const wxString& resourceName, int desiredWidth, int desiredHeight)
{
    UnRef();

    OSType theId = 0 ;

    if ( resourceName == wxT("wxICON_INFORMATION") )
    {
        theId = kAlertNoteIcon ;
    }
    else if ( resourceName == wxT("wxICON_QUESTION") )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == wxT("wxICON_WARNING") )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == wxT("wxICON_ERROR") )
    {
        theId = kAlertStopIcon ;
    }
    else if ( resourceName == wxT("wxICON_FOLDER") )
    {
        theId = kGenericFolderIcon ;
    }
    else if ( resourceName == wxT("wxICON_FOLDER_OPEN") )
    {
        theId = kOpenFolderIcon ;
    }
    else if ( resourceName == wxT("wxICON_NORMAL_FILE") )
    {
        theId = kGenericDocumentIcon ;
    }
    else if ( resourceName == wxT("wxICON_EXECUTABLE_FILE") )
    {
        theId = kGenericApplicationIcon ;
    }
    else if ( resourceName == wxT("wxICON_CDROM") )
    {
        theId = kGenericCDROMIcon ;
    }
    else if ( resourceName == wxT("wxICON_FLOPPY") )
    {
        theId = kGenericFloppyIcon ;
    }
    else if ( resourceName == wxT("wxICON_HARDDISK") )
    {
        theId = kGenericHardDiskIcon ;
    }
    else if ( resourceName == wxT("wxICON_REMOVABLE") )
    {
        theId = kGenericRemovableMediaIcon ;
    }
    else if ( resourceName == wxT("wxICON_DELETE") )
    {
        theId = kToolbarDeleteIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_BACK") )
    {
        theId = kBackwardArrowIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_FORWARD") )
    {
        theId = kForwardArrowIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_HOME") )
    {
        theId = kToolbarHomeIcon ;
    }
    else if ( resourceName == wxT("wxICON_HELP_SETTINGS") )
    {
        theId = kGenericFontIcon ;
    }
    else if ( resourceName == wxT("wxICON_HELP_PAGE") )
    {
        theId = kGenericDocumentIcon ;
    }

    if ( theId != 0 )
    {
        IconRef iconRef = NULL ;
        verify_noerr( GetIconRef( kOnSystemDisk, kSystemIconsCreator, theId, &iconRef ) ) ;
        if ( iconRef )
        {
            m_refData = new wxIconRefData( (WXHICON) iconRef, desiredWidth, desiredHeight ) ;
            return true ;
        }
    }

    return false;
}

// Load an icon of type 'icns' by resource by name
// The resource must exist in one of the currently accessible bundles
// (usually this means the application bundle for the current application)
// Return true on success, false otherwise
bool wxIcon::LoadIconFromBundleResource(const wxString& resourceName, int desiredWidth, int desiredHeight)
{
    UnRef();

    IconRef iconRef = NULL ;

    // first look in the resource fork
    if ( iconRef == NULL )
    {
        Str255 theName ;

        wxMacStringToPascal( resourceName , theName ) ;
        Handle resHandle = GetNamedResource( 'icns' , theName ) ;
        if ( resHandle != 0L )
        {
            IconFamilyHandle iconFamily = (IconFamilyHandle) resHandle ;
            OSStatus err = GetIconRefFromIconFamilyPtr( *iconFamily, GetHandleSize((Handle) iconFamily), &iconRef );

            if ( err != noErr )
            {
                wxFAIL_MSG("Error when constructing icon ref");
            }

            ReleaseResource( resHandle ) ;
        }
    }

    if ( iconRef )
    {
        m_refData = new wxIconRefData( (WXHICON) iconRef, desiredWidth, desiredHeight );
        return true;
    }

   return false;
}

// Load an icon from an icon file using the underlying OS X API
// The icon file must be in a format understood by the OS
// Return true for success, false otherwise
bool wxIcon::LoadIconFromFile(const wxString& filename, int desiredWidth, int desiredHeight)
{
    UnRef();

    OSStatus err;
    bool result = false;

    // Get a file system reference
    FSRef fsRef;
    err = FSPathMakeRef( (const wxUint8*)filename.utf8_str().data(), &fsRef, NULL );

    if( err != noErr )
        return false;

    // Get a handle on the icon family
    IconFamilyHandle iconFamily;
    err = ReadIconFromFSRef( &fsRef, &iconFamily );

    if( err != noErr )
        return false;

    // Get the icon reference itself
    IconRef iconRef;
    err = GetIconRefFromIconFamilyPtr( *iconFamily, GetHandleSize((Handle) iconFamily), &iconRef );

    if( err == noErr )
    {
        // If everthing is OK, assign m_refData
        m_refData = new wxIconRefData( (WXHICON) iconRef, desiredWidth, desiredHeight );
        result = true;
    }

    // Release the iconFamily before returning
    ReleaseResource( (Handle) iconFamily );
    return result;
}

// Load an icon from a file using functionality from wxWidgets
// A suitable bitmap handler (or image handler) must be available
// Return true on success, false otherwise
bool wxIcon::LoadIconAsBitmap(const wxString& filename, wxBitmapType type, int desiredWidth, int desiredHeight)
{
    UnRef();

    wxBitmapHandler *handler = wxBitmap::FindHandler( type );

    if ( handler )
    {
        wxBitmap bmp ;
        if ( handler->LoadFile( &bmp , filename, type, desiredWidth, desiredHeight ))
        {
            CopyFromBitmap( bmp ) ;
            return true ;
        }
    }

#if wxUSE_IMAGE
    else
    {
        wxImage loadimage( filename, type );
        if (loadimage.Ok())
        {
            if ( desiredWidth == -1 )
                desiredWidth = loadimage.GetWidth() ;
            if ( desiredHeight == -1 )
                desiredHeight = loadimage.GetHeight() ;
            if ( desiredWidth != loadimage.GetWidth() || desiredHeight != loadimage.GetHeight() )
                loadimage.Rescale( desiredWidth , desiredHeight ) ;

            wxBitmap bmp( loadimage );
            CopyFromBitmap( bmp ) ;

            return true;
        }
    }
#endif

    return false;
}


void wxIcon::CopyFromBitmap( const wxBitmap& bmp )
{
    UnRef() ;

    // as the bitmap owns that ref, we have to acquire it as well
    IconRef iconRef = bmp.CreateIconRef() ;
    m_refData = new wxIconRefData( (WXHICON) iconRef, bmp.GetWidth(), bmp.GetHeight()  ) ;
}

IMPLEMENT_DYNAMIC_CLASS(wxICONResourceHandler, wxBitmapHandler)

bool  wxICONResourceHandler::LoadFile(
    wxBitmap *bitmap, const wxString& name, wxBitmapType WXUNUSED(flags),
    int desiredWidth, int desiredHeight )
{
    wxIcon icon ;
    if ( icon.LoadFile( name , wxBITMAP_TYPE_ICON_RESOURCE , desiredWidth , desiredHeight ) )
    {
        bitmap->CopyFromIcon( icon ) ;
        return bitmap->Ok() ;
    }
    return false;
}

#endif

