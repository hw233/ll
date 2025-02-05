///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/listbkg.cpp
// Purpose:     generic implementation of wxListbook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// RCS-ID:      $Id: listbkg.cpp 63632 2010-03-05 23:55:19Z VZ $
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOOK

#include "wx/listbook.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif

#include "wx/listctrl.h"
#include "wx/statline.h"
#include "wx/imaglist.h"

// FIXME: native OS X wxListCtrl hangs if this code is used for it so disable
//        it for now
#if !defined(__WXMAC__)
    #define CAN_USE_REPORT_VIEW
#endif

// ----------------------------------------------------------------------------
// various wxWidgets macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListbook, wxBookCtrlBase)

wxDEFINE_EVENT( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,  wxBookCtrlEvent );

BEGIN_EVENT_TABLE(wxListbook, wxBookCtrlBase)
    EVT_SIZE(wxListbook::OnSize)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, wxListbook::OnListSelected)
END_EVENT_TABLE()

// ============================================================================
// wxListbook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListbook creation
// ----------------------------------------------------------------------------

void wxListbook::Init()
{
    m_selection = wxNOT_FOUND;
}

bool
wxListbook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
#ifdef __WXMAC__
        style |= wxBK_TOP;
#else // !__WXMAC__
        style |= wxBK_LEFT;
#endif // __WXMAC__/!__WXMAC__
    }

    // no border for this control, it doesn't look nice together with
    // wxListCtrl border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_bookctrl = new wxListView
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_SINGLE_SEL |
#ifdef CAN_USE_REPORT_VIEW
                    GetListCtrlReportViewFlags()
#else // !CAN_USE_REPORT_VIEW
                    GetListCtrlIconViewFlags()
#endif // CAN_USE_REPORT_VIEW/!CAN_USE_REPORT_VIEW
                 );

#ifdef CAN_USE_REPORT_VIEW
    GetListView()->InsertColumn(0, wxT("Pages"));
#endif // CAN_USE_REPORT_VIEW

#ifdef __WXMSW__
    // On XP with themes enabled the GetViewRect used in GetControllerSize() to
    // determine the space needed for the list view will incorrectly return
    // (0,0,0,0) the first time.  So send a pending event so OnSize will be
    // called again after the window is ready to go.  Technically we don't
    // need to do this on non-XP windows, but if things are already sized
    // correctly then nothing changes and so there is no harm.
    wxSizeEvent evt;
    GetEventHandler()->AddPendingEvent(evt);
#endif
    return true;
}

// ----------------------------------------------------------------------------
// wxListCtrl flags
// ----------------------------------------------------------------------------

long wxListbook::GetListCtrlIconViewFlags() const
{
    return (IsVertical() ? wxLC_ALIGN_LEFT : wxLC_ALIGN_TOP) | wxLC_ICON;
}

#ifdef CAN_USE_REPORT_VIEW

long wxListbook::GetListCtrlReportViewFlags() const
{
    return wxLC_REPORT | wxLC_NO_HEADER;
}

#endif // CAN_USE_REPORT_VIEW

// ----------------------------------------------------------------------------
// wxListbook geometry management
// ----------------------------------------------------------------------------

void wxListbook::OnSize(wxSizeEvent& event)
{
    // arrange the icons before calling SetClientSize(), otherwise it wouldn't
    // account for the scrollbars the list control might need and, at least
    // under MSW, we'd finish with an ugly looking list control with both
    // vertical and horizontal scrollbar (with one of them being added because
    // the other one is not accounted for in client size computations)
    wxListView * const list = GetListView();
    if ( list )
        list->Arrange();

    event.Skip();
}

int wxListbook::HitTest(const wxPoint& pt, long *flags) const
{
    int pagePos = wxNOT_FOUND;

    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    // convert from listbook control coordinates to list control coordinates
    const wxListView * const list = GetListView();
    const wxPoint listPt = list->ScreenToClient(ClientToScreen(pt));

    // is the point inside list control?
    if ( wxRect(list->GetSize()).Contains(listPt) )
    {
        int flagsList;
        pagePos = list->HitTest(listPt, flagsList);

        if ( flags )
        {
            if ( pagePos != wxNOT_FOUND )
                *flags = 0;

            if ( flagsList & (wxLIST_HITTEST_ONITEMICON |
                              wxLIST_HITTEST_ONITEMSTATEICON ) )
                *flags |= wxBK_HITTEST_ONICON;

            if ( flagsList & wxLIST_HITTEST_ONITEMLABEL )
                *flags |= wxBK_HITTEST_ONLABEL;
        }
    }
    else // not over list control at all
    {
        if ( flags && GetPageRect().Contains(pt) )
            *flags |= wxBK_HITTEST_ONPAGE;
    }

    return pagePos;
}

void wxListbook::UpdateSize()
{
    // we should find a more elegant way to force a layout than generating this
    // dummy event
    wxSizeEvent sz(GetSize(), GetId());
    GetEventHandler()->ProcessEvent(sz);
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxListbook::SetPageText(size_t n, const wxString& strText)
{
    GetListView()->SetItemText(n, strText);

    return true;
}

wxString wxListbook::GetPageText(size_t n) const
{
    return GetListView()->GetItemText(n);
}

int wxListbook::GetPageImage(size_t n) const
{
    wxListItem item;
    item.SetId(n);

    if (GetListView()->GetItem(item))
    {
       return item.GetImage();
    }
    else
    {
       return wxNOT_FOUND;
    }
}

bool wxListbook::SetPageImage(size_t n, int imageId)
{
    return GetListView()->SetItemImage(n, imageId);
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxListbook::SetImageList(wxImageList *imageList)
{
#ifdef CAN_USE_REPORT_VIEW
    wxListView * const list = GetListView();

    // If imageList presence has changed, we update the list control view
    if ( (imageList != NULL) != (GetImageList() != NULL) )
    {
        // Preserve the selection which is lost when changing the mode
        const int oldSel = GetSelection();

        // Update the style to use icon view for images, report view otherwise
        long style = wxLC_SINGLE_SEL;
        if ( imageList )
        {
            style |= GetListCtrlIconViewFlags();
        }
        else // no image list
        {
            style |= GetListCtrlReportViewFlags();
        }

        list->SetWindowStyleFlag(style);
        if ( !imageList )
            list->InsertColumn(0, wxT("Pages"));

        // Restore selection
        if ( oldSel != wxNOT_FOUND )
           SetSelection(oldSel);
    }

    list->SetImageList(imageList, wxIMAGE_LIST_NORMAL);
#endif // CAN_USE_REPORT_VIEW

    wxBookCtrlBase::SetImageList(imageList);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxListbook::UpdateSelectedPage(size_t newsel)
{
    m_selection = newsel;
    GetListView()->Select(newsel);
    GetListView()->Focus(newsel);
}

int wxListbook::GetSelection() const
{
    return m_selection;
}

wxBookCtrlEvent* wxListbook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, m_windowId);
}

void wxListbook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED);
}


// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxListbook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( !wxBookCtrlBase::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    GetListView()->InsertItem(n, text, imageId);

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(n) <= m_selection )
    {
        // one extra page added
        m_selection++;
        GetListView()->Select(m_selection);
        GetListView()->Focus(m_selection);
    }

    // some page should be selected: either this one or the first one if there
    // is still no selection
    int selNew = -1;
    if ( bSelect )
        selNew = n;
    else if ( m_selection == -1 )
        selNew = 0;

    if ( selNew != m_selection )
        page->Hide();

    if ( selNew != -1 )
        SetSelection(selNew);

    UpdateSize();

    return true;
}

wxWindow *wxListbook::DoRemovePage(size_t page)
{
    const size_t page_count = GetPageCount();
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        GetListView()->DeleteItem(page);

        if (m_selection >= (int)page)
        {
            // force new sel valid if possible
            int sel = m_selection - 1;
            if (page_count == 1)
                sel = wxNOT_FOUND;
            else if ((page_count == 2) || (sel == -1))
                sel = 0;

            // force sel invalid if deleting current page - don't try to hide it
            m_selection = (m_selection == (int)page) ? wxNOT_FOUND : m_selection - 1;

            if ((sel != wxNOT_FOUND) && (sel != m_selection))
                SetSelection(sel);
        }

        GetListView()->Arrange();
        UpdateSize();
    }

    return win;
}


bool wxListbook::DeleteAllPages()
{
    GetListView()->DeleteAllItems();
    if (!wxBookCtrlBase::DeleteAllPages())
        return false;

    m_selection = -1;

    UpdateSize();

    return true;
}

// ----------------------------------------------------------------------------
// wxListbook events
// ----------------------------------------------------------------------------

void wxListbook::OnListSelected(wxListEvent& eventList)
{
    if ( eventList.GetEventObject() != m_bookctrl )
    {
        eventList.Skip();
        return;
    }

    const int selNew = eventList.GetIndex();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    SetSelection(selNew);

    // change wasn't allowed, return to previous state
    if (m_selection != selNew)
    {
        GetListView()->Select(m_selection);
        GetListView()->Focus(m_selection);
    }
}

#endif // wxUSE_LISTBOOK
