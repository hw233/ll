///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statusbar.cpp
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// RCS-ID:      $Id: statusbar.cpp 64656 2010-06-20 18:18:23Z VZ $
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STATUSBAR && wxUSE_NATIVE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/frame.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/control.h"
#endif

#include "wx/msw/private.h"
#include "wx/tooltip.h"
#include <windowsx.h>

#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

// no idea for a default width, just choose something
static const int DEFAULT_FIELD_WIDTH = 25;

} // anonymous namespace

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// windowsx.h and commctrl.h don't define those, so we do it here
#define StatusBar_SetParts(h, n, w) SendMessage(h, SB_SETPARTS, (WPARAM)n, (LPARAM)w)
#define StatusBar_SetText(h, n, t)  SendMessage(h, SB_SETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)t)
#define StatusBar_GetTextLen(h, n)  LOWORD(SendMessage(h, SB_GETTEXTLENGTH, (WPARAM)n, 0))
#define StatusBar_GetText(h, n, s)  LOWORD(SendMessage(h, SB_GETTEXT, (WPARAM)n, (LPARAM)(LPTSTR)s))

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBar class
// ----------------------------------------------------------------------------

wxStatusBar::wxStatusBar()
{
    SetParent(NULL);
    m_hWnd = 0;
    m_windowId = 0;
    m_pDC = NULL;
}

bool wxStatusBar::Create(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( parent, false, "status bar must have a parent" );

    SetName(name);
    SetWindowStyleFlag(style);
    SetParent(parent);

    parent->AddChild(this);

    m_windowId = id == wxID_ANY ? NewControlId() : id;

    DWORD wstyle = WS_CHILD | WS_VISIBLE;

    if ( style & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;

    // wxSTB_SIZEGRIP is part of our default style but it doesn't make sense to
    // show size grip if this is the status bar of a non-resizeable TLW so turn
    // it off in such case
    if ( parent->IsTopLevel() && !parent->HasFlag(wxRESIZE_BORDER) )
        style &= ~wxSTB_SIZEGRIP;

    // setting SBARS_SIZEGRIP is perfectly useless: it's always on by default
    // (at least in the version of comctl32.dll I'm using), and the only way to
    // turn it off is to use CCS_TOP style - as we position the status bar
    // manually anyhow (see DoMoveWindow), use CCS_TOP style if wxSTB_SIZEGRIP
    // is not given
    if ( !(style & wxSTB_SIZEGRIP) )
    {
        wstyle |= CCS_TOP;
    }
    else
    {
#ifndef __WXWINCE__
        // may be some versions of comctl32.dll do need it - anyhow, it won't
        // do any harm
        wstyle |= SBARS_SIZEGRIP;
#endif
    }

    m_hWnd = CreateWindow
             (
                STATUSCLASSNAME,
                wxT(""),
                wstyle,
                0, 0, 0, 0,
                GetHwndOf(parent),
                (HMENU)wxUIntToPtr(m_windowId.GetValue()),
                wxGetInstance(),
                NULL
             );
    if ( m_hWnd == 0 )
    {
        wxLogSysError(_("Failed to create a status bar."));

        return false;
    }

    SetFieldsCount(1);
    SubclassWin(m_hWnd);

    // cache the DC instance used by DoUpdateStatusText:
    // NOTE: create the DC before calling InheritAttributes() since
    //       it may result in a call to our SetFont()
    m_pDC = new wxClientDC(this);

    InheritAttributes();

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    // we must refresh the frame size when the statusbar is created, because
    // its client area might change
    //
    // notice that we must post the event, not send it, as the frame doesn't
    // know that we're its status bar yet so laying it out right now wouldn't
    // work correctly, we need to wait until we return to the main loop
    PostSizeEventToParent();

    return true;
}

wxStatusBar::~wxStatusBar()
{
    // we must refresh the frame size when the statusbar is deleted but the
    // frame is not - otherwise statusbar leaves a hole in the place it used to
    // occupy
    PostSizeEventToParent();

    // delete existing tooltips
    for (size_t i=0; i<m_tooltips.size(); i++)
    {
        wxDELETE(m_tooltips[i]);
    }

    wxDELETE(m_pDC);
}

bool wxStatusBar::SetFont(const wxFont& font)
{
    if (!wxWindow::SetFont(font))
        return false;

    if (m_pDC) m_pDC->SetFont(font);
    return true;
}

void wxStatusBar::SetFieldsCount(int nFields, const int *widths)
{
    // this is a Windows limitation
    wxASSERT_MSG( (nFields > 0) && (nFields < 255), "too many fields" );

    wxStatusBarBase::SetFieldsCount(nFields, widths);

    MSWUpdateFieldsWidths();

    // keep in synch also our m_tooltips array

    // reset all current tooltips
    for (size_t i=0; i<m_tooltips.size(); i++)
    {
        wxDELETE(m_tooltips[i]);
    }

    // shrink/expand the array:
    m_tooltips.resize(m_panes.GetCount(), NULL);
}

void wxStatusBar::SetStatusWidths(int n, const int widths[])
{
    wxStatusBarBase::SetStatusWidths(n, widths);

    MSWUpdateFieldsWidths();
}

void wxStatusBar::MSWUpdateFieldsWidths()
{
    if ( m_panes.IsEmpty() )
        return;

    const int count = m_panes.GetCount();

    const int extraWidth = MSWGetBorderWidth() + MSWGetMetrics().textMargin;

    // compute the effectively available amount of space:
    int widthAvailable = GetClientSize().x;     // start with the entire width
    widthAvailable -= extraWidth*(count - 1);   // extra space between fields
    widthAvailable -= MSWGetMetrics().textMargin;   // and for the last field

    if ( HasFlag(wxSTB_SIZEGRIP) )
        widthAvailable -= MSWGetMetrics().gripWidth;

    // distribute the available space (client width) among the various fields:

    wxArrayInt widthsAbs = CalculateAbsWidths(widthAvailable);


    // update the field widths in the native control:

    int *pWidths = new int[count];

    int nCurPos = 0;
    for ( int i = 0; i < count; i++ )
    {
        nCurPos += widthsAbs[i] + extraWidth;
        pWidths[i] = nCurPos;
    }

    if ( !StatusBar_SetParts(GetHwnd(), count, pWidths) )
    {
        wxLogLastError("StatusBar_SetParts");
    }

    delete [] pWidths;


    // FIXME: we may want to call DoUpdateStatusText() here since we may need to (de)ellipsize status texts
}

void wxStatusBar::DoUpdateStatusText(int nField)
{
    if (!m_pDC)
        return;

    // Get field style, if any
    int style;
    switch(m_panes[nField].GetStyle())
    {
    case wxSB_RAISED:
        style = SBT_POPOUT;
        break;
    case wxSB_FLAT:
        style = SBT_NOBORDERS;
        break;

    case wxSB_NORMAL:
    default:
        style = 0;
        break;
    }

    wxRect rc;
    GetFieldRect(nField, rc);

    const int maxWidth = rc.GetWidth() - MSWGetMetrics().textMargin;

    wxString text = GetStatusText(nField);

    // do we need to ellipsize this string?
    wxEllipsizeMode ellmode = (wxEllipsizeMode)-1;
    if (HasFlag(wxSTB_ELLIPSIZE_START)) ellmode = wxELLIPSIZE_START;
    else if (HasFlag(wxSTB_ELLIPSIZE_MIDDLE)) ellmode = wxELLIPSIZE_MIDDLE;
    else if (HasFlag(wxSTB_ELLIPSIZE_END)) ellmode = wxELLIPSIZE_END;

    if (ellmode == (wxEllipsizeMode)-1)
    {
        // if we have the wxSTB_SHOW_TIPS we must set the ellipsized flag even if
        // we don't ellipsize the text but just truncate it
        if (HasFlag(wxSTB_SHOW_TIPS))
            SetEllipsizedFlag(nField, m_pDC->GetTextExtent(text).GetWidth() > maxWidth);
    }
    else
    {
        text = wxControl::Ellipsize(text,
                                     *m_pDC,
                                     ellmode,
                                     maxWidth,
                                     wxELLIPSIZE_FLAGS_EXPAND_TABS);

        // update the ellipsization status for this pane; this is used later to
        // decide whether a tooltip should be shown or not for this pane
        // (if we have wxSTB_SHOW_TIPS)
        SetEllipsizedFlag(nField, text != GetStatusText(nField));
    }

    // Set the status text in the native control passing both field number and style.
    // NOTE: MSDN library doesn't mention that nField and style have to be 'ORed'
    if ( !StatusBar_SetText(GetHwnd(), nField | style, text.wx_str()) )
    {
        wxLogLastError("StatusBar_SetText");
    }

    if (HasFlag(wxSTB_SHOW_TIPS))
    {
        wxASSERT(m_tooltips.size() == m_panes.GetCount());

        if (m_tooltips[nField])
        {
            if (GetField(nField).IsEllipsized())
            {
                // update the rect of this tooltip:
                m_tooltips[nField]->SetRect(rc);

                // update also the text:
                m_tooltips[nField]->SetTip(GetStatusText(nField));
            }
            else
            {
                // delete the tooltip associated with this pane; it's not needed anymore
                wxDELETE(m_tooltips[nField]);
            }
        }
        else
        {
            // create a new tooltip for this pane if needed
            if (GetField(nField).IsEllipsized())
                m_tooltips[nField] = new wxToolTip(this, nField, GetStatusText(nField), rc);
            //else: leave m_tooltips[nField]==NULL
        }
    }
}

wxStatusBar::MSWBorders wxStatusBar::MSWGetBorders() const
{
    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    MSWBorders borders;
    borders.horz = aBorders[0];
    borders.vert = aBorders[1];
    borders.between = aBorders[2];
    return borders;
}

int wxStatusBar::GetBorderX() const
{
    return MSWGetBorders().horz;
}

int wxStatusBar::GetBorderY() const
{
    return MSWGetBorders().vert;
}

int wxStatusBar::MSWGetBorderWidth() const
{
    return MSWGetBorders().between;
}

/* static */
const wxStatusBar::MSWMetrics& wxStatusBar::MSWGetMetrics()
{
    static MSWMetrics s_metrics = { 0 };
    if ( !s_metrics.textMargin )
    {
        // Grip size should be self explanatory (the only problem with it is
        // that it's hard coded as we don't know how to find its size using
        // API) but the margin might merit an explanation: Windows offsets the
        // text drawn in status bar panes so we need to take this extra margin
        // into account to make sure the text drawn by user fits inside the
        // pane. Notice that it's not the value returned by SB_GETBORDERS
        // which, at least on this Windows 2003 system, returns {0, 2, 2}
        if ( wxUxThemeEngine::GetIfActive() )
        {
            s_metrics.gripWidth = 20;
            s_metrics.textMargin = 8;
        }
        else // classic/unthemed look
        {
            s_metrics.gripWidth = 18;
            s_metrics.textMargin = 4;
        }
    }

    return s_metrics;
}

void wxStatusBar::SetMinHeight(int height)
{
    SendMessage(GetHwnd(), SB_SETMINHEIGHT, height + 2*GetBorderY(), 0);

    // we have to send a (dummy) WM_SIZE to redraw it now
    SendMessage(GetHwnd(), WM_SIZE, 0, 0);
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    wxCHECK_MSG( (i >= 0) && ((size_t)i < m_panes.GetCount()), false,
                 "invalid statusbar field index" );

    RECT r;
    if ( !::SendMessage(GetHwnd(), SB_GETRECT, i, (LPARAM)&r) )
    {
        wxLogLastError("SendMessage(SB_GETRECT)");
    }

#if wxUSE_UXTHEME
    wxUxThemeHandle theme(const_cast<wxStatusBar*>(this), L"Status");
    if ( theme )
    {
        // by default Windows has a 2 pixel border to the right of the left
        // divider (or it could be a bug) but it looks wrong so remove it
        if ( i != 0 )
        {
            r.left -= 2;
        }

        wxUxThemeEngine::Get()->GetThemeBackgroundContentRect(theme, NULL,
                                                              1 /* SP_PANE */, 0,
                                                              &r, &r);
    }
#endif

    wxCopyRECTToRect(r, rect);

    // Windows seems to under-report the size of the last field rectangle,
    // presumably in order to prevent the buggy applications from overflowing
    // onto the size grip but we want to return the real size to wx users
    if ( HasFlag(wxSTB_SIZEGRIP) && i == (int)m_panes.GetCount() - 1 )
    {
        rect.width += MSWGetMetrics().gripWidth - MSWGetBorderWidth();
    }

    return true;
}

wxSize wxStatusBar::DoGetBestSize() const
{
    const MSWBorders borders = MSWGetBorders();

    // calculate width
    int width = 0;
    for ( size_t i = 0; i < m_panes.GetCount(); ++i )
    {
        int widthField =
            m_bSameWidthForAllPanes ? DEFAULT_FIELD_WIDTH : m_panes[i].GetWidth();
        if ( widthField >= 0 )
        {
            width += widthField;
        }
        else
        {
            // variable width field, its width is really a proportion
            // related to the other fields
            width += -widthField*DEFAULT_FIELD_WIDTH;
        }

        // add the space between fields
        width += borders.between;
    }

    if ( !width )
    {
        // still need something even for an empty status bar
        width = 2*DEFAULT_FIELD_WIDTH;
    }

    // calculate height
    int height;
    wxGetCharSize(GetHWND(), NULL, &height, GetFont());
    height = EDIT_HEIGHT_FROM_CHAR_HEIGHT(height);
    height += borders.vert;

    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

void wxStatusBar::DoMoveWindow(int x, int y, int width, int height)
{
    if ( GetParent()->IsSizeDeferred() )
    {
        wxWindowMSW::DoMoveWindow(x, y, width, height);
    }
    else
    {
        // parent pos/size isn't deferred so do it now but don't send
        // WM_WINDOWPOSCHANGING since we don't want to change pos/size later
        // we must use SWP_NOCOPYBITS here otherwise it paints incorrectly
        // if other windows are size deferred
        ::SetWindowPos(GetHwnd(), NULL, x, y, width, height,
                       SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
#ifndef __WXWINCE__
                       | SWP_NOCOPYBITS | SWP_NOSENDCHANGING
#endif
                       );
    }

    // adjust fields widths to the new size
    MSWUpdateFieldsWidths();

    // we have to trigger wxSizeEvent if there are children window in status
    // bar because GetFieldRect returned incorrect (not updated) values up to
    // here, which almost certainly resulted in incorrectly redrawn statusbar
    if ( m_children.GetCount() > 0 )
    {
        wxSizeEvent event(GetClientSize(), m_windowId);
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

void wxStatusBar::SetStatusStyles(int n, const int styles[])
{
    wxStatusBarBase::SetStatusStyles(n, styles);

    if (n != (int)m_panes.GetCount())
        return;

    for (int i = 0; i < n; i++)
    {
        int style;
        switch(styles[i])
        {
        case wxSB_RAISED:
            style = SBT_POPOUT;
            break;
        case wxSB_FLAT:
            style = SBT_NOBORDERS;
            break;
        case wxSB_NORMAL:
        default:
            style = 0;
            break;
        }

        // The SB_SETTEXT message is both used to set the field's text as well as
        // the fields' styles.
        // NOTE: MSDN library doesn't mention that nField and style have to be 'ORed'
        wxString text = GetStatusText(i);
        if (!StatusBar_SetText(GetHwnd(), style | i, text.wx_str()))
        {
            wxLogLastError("StatusBar_SetText");
        }
    }
}

WXLRESULT
wxStatusBar::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
#ifndef __WXWINCE__
    if ( nMsg == WM_WINDOWPOSCHANGING )
    {
        WINDOWPOS *lpPos = (WINDOWPOS *)lParam;
        int x, y, w, h;
        GetPosition(&x, &y);
        GetSize(&w, &h);

        // we need real window coords and not wx client coords
        AdjustForParentClientOrigin(x, y);

        lpPos->x  = x;
        lpPos->y  = y;
        lpPos->cx = w;
        lpPos->cy = h;

        return 0;
    }

    if ( nMsg == WM_NCLBUTTONDOWN )
    {
        // if hit-test is on gripper then send message to TLW to begin
        // resizing. It is possible to send this message to any window.
        if ( wParam == HTBOTTOMRIGHT )
        {
            wxWindow *win;

            for ( win = GetParent(); win; win = win->GetParent() )
            {
                if ( win->IsTopLevel() )
                {
                    SendMessage(GetHwndOf(win), WM_NCLBUTTONDOWN,
                                wParam, lParam);

                    return 0;
                }
            }
        }
    }
#endif

    bool needsEllipsization = HasFlag(wxSTB_ELLIPSIZE_START) ||
                              HasFlag(wxSTB_ELLIPSIZE_MIDDLE) ||
                              HasFlag(wxSTB_ELLIPSIZE_END);
    if ( nMsg == WM_SIZE && needsEllipsization )
    {
        for (int i=0; i<GetFieldsCount(); i++)
            DoUpdateStatusText(i);
            // re-set the field text, in case we need to ellipsize
            // (or de-ellipsize) some parts of it
    }

    return wxStatusBarBase::MSWWindowProc(nMsg, wParam, lParam);
}

#if wxUSE_TOOLTIPS
bool wxStatusBar::MSWProcessMessage(WXMSG* pMsg)
{
    if ( HasFlag(wxSTB_SHOW_TIPS) )
    {
        // for a tooltip to be shown, we need to relay mouse events to it;
        // this is typically done by wxWindowMSW::MSWProcessMessage but only
        // if wxWindow::m_tooltip pointer is non-NULL.
        // Since wxStatusBar has multiple tooltips for a single HWND, it keeps
        // wxWindow::m_tooltip == NULL and then relays mouse events here:
        MSG *msg = (MSG *)pMsg;
        if ( msg->message == WM_MOUSEMOVE )
            wxToolTip::RelayEvent(pMsg);
    }

    return wxWindow::MSWProcessMessage(pMsg);
}

bool wxStatusBar::MSWOnNotify(int WXUNUSED(idCtrl), WXLPARAM lParam, WXLPARAM* WXUNUSED(result))
{
    if ( HasFlag(wxSTB_SHOW_TIPS) )
    {
        // see comment in wxStatusBar::MSWProcessMessage for more info;
        // basically we need to override wxWindow::MSWOnNotify because
        // we have wxWindow::m_tooltip always NULL but we still use tooltips...

        NMHDR* hdr = (NMHDR *)lParam;

        wxString str;
        if (hdr->idFrom < m_tooltips.size() && m_tooltips[hdr->idFrom])
            str = m_tooltips[hdr->idFrom]->GetTip();

        if ( HandleTooltipNotify(hdr->code, lParam, str))
        {
            // processed
            return true;
        }
    }

    return false;
}
#endif // wxUSE_TOOLTIPS

#endif // wxUSE_STATUSBAR && wxUSE_NATIVE_STATUSBAR
