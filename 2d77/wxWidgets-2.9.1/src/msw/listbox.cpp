///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/listbox.cpp
// Purpose:     wxListBox
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (owner drawn stuff)
// Created:
// RCS-ID:      $Id: listbox.cpp 64500 2010-06-05 22:58:13Z VZ $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/font.h"
    #include "wx/dc.h"
    #include "wx/utils.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/dc.h"

#include <windowsx.h>

#if wxUSE_OWNER_DRAWN
    #include  "wx/ownerdrw.h"
#endif

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxListBoxStyle )

wxBEGIN_FLAGS( wxListBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxLB_SINGLE)
    wxFLAGS_MEMBER(wxLB_MULTIPLE)
    wxFLAGS_MEMBER(wxLB_EXTENDED)
    wxFLAGS_MEMBER(wxLB_HSCROLL)
    wxFLAGS_MEMBER(wxLB_ALWAYS_SB)
    wxFLAGS_MEMBER(wxLB_NEEDED_SB)
    wxFLAGS_MEMBER(wxLB_NO_SB)
    wxFLAGS_MEMBER(wxLB_SORT)

wxEND_FLAGS( wxListBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxListBox, wxControlWithItems,"wx/listbox.h")

wxBEGIN_PROPERTIES_TABLE(wxListBox)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_LISTBOX_SELECTED , wxCommandEvent )
    wxEVENT_PROPERTY( DoubleClick , wxEVT_COMMAND_LISTBOX_DOUBLECLICKED , wxCommandEvent )

    wxPROPERTY( Font , wxFont , SetFont , GetFont  , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_COLLECTION( Choices , wxArrayString , wxString , AppendString , GetStrings, 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Selection ,int, SetSelection, GetSelection, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY_FLAGS( WindowStyle , wxListBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxListBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_4( wxListBox , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size )
#else
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControlWithItems)
#endif

/*
TODO PROPERTIES
    selection
    content
        item
*/

// ============================================================================
// list box item declaration and implementation
// ============================================================================

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(wxListBox *parent)
        { m_parent = parent; }

    wxListBox *GetParent() const
        { return m_parent; }

    int GetIndex() const
        { return m_parent->GetItemIndex(const_cast<wxListBoxItem*>(this)); }

    wxString GetName() const
        { return m_parent->GetString(GetIndex()); }

private:
    wxListBox *m_parent;
};

wxOwnerDrawn *wxListBox::CreateLboxItem(size_t WXUNUSED(n))
{
    return new wxListBoxItem(this);
}

#endif  //USE_OWNER_DRAWN

// ============================================================================
// list box control implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxListBox::Init()
{
    m_noItems = 0;
    m_updateHorizontalExtent = false;
    m_selectedByKeyboard = false;
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    // initialize base class fields
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(wxT("LISTBOX"), wxEmptyString, pos, size) )
    {
        // control creation failed
        return false;
    }

    // initialize the contents
    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    // now we can compute our best size correctly, so do it again
    SetInitialSize(size);

    return true;
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

wxListBox::~wxListBox()
{
    Clear();
}

WXDWORD wxListBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // we always want to get the notifications
    msStyle |= LBS_NOTIFY;

    // without this style, you get unexpected heights, so e.g. constraint
    // layout doesn't work properly
    msStyle |= LBS_NOINTEGRALHEIGHT;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  wxT("only one of listbox selection modes can be specified") );

    if ( style & wxLB_MULTIPLE )
        msStyle |= LBS_MULTIPLESEL;
    else if ( style & wxLB_EXTENDED )
        msStyle |= LBS_EXTENDEDSEL;

    wxASSERT_MSG( !(style & wxLB_ALWAYS_SB) || !(style & wxLB_NO_SB),
                  wxT( "Conflicting styles wxLB_ALWAYS_SB and wxLB_NO_SB." ) );

    if ( !(style & wxLB_NO_SB) )
    {
        msStyle |= WS_VSCROLL;
        if ( style & wxLB_ALWAYS_SB )
            msStyle |= LBS_DISABLENOSCROLL;
    }

    if ( m_windowStyle & wxLB_HSCROLL )
        msStyle |= WS_HSCROLL;
    if ( m_windowStyle & wxLB_SORT )
        msStyle |= LBS_SORT;

#if wxUSE_OWNER_DRAWN && !defined(__WXWINCE__)
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // we don't support LBS_OWNERDRAWVARIABLE yet and we also always put
        // the strings in the listbox for simplicity even though we could have
        // avoided it in this case
        msStyle |= LBS_OWNERDRAWFIXED | LBS_HASSTRINGS;
    }
#endif // wxUSE_OWNER_DRAWN

    return msStyle;
}

void wxListBox::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_updateHorizontalExtent)
    {
        SetHorizontalExtent(wxEmptyString);
        m_updateHorizontalExtent = false;
    }
}

void wxListBox::MSWOnItemsChanged()
{
    // we need to do two things when items change: update their max horizontal
    // extent so that horizontal scrollbar could be shown or hidden as
    // appropriate and also invlaidate the best size
    //
    // updating the max extent is slow (it's an O(N) operation) and so we defer
    // it until the idle time but the best size should be invalidated
    // immediately doing it in idle time is too late -- layout using incorrect
    // old best size will have been already done by then

    m_updateHorizontalExtent = true;

    InvalidateBestSize();
}

// ----------------------------------------------------------------------------
// implementation of wxListBoxBase methods
// ----------------------------------------------------------------------------

void wxListBox::DoSetFirstItem(int N)
{
    wxCHECK_RET( IsValid(N),
                 wxT("invalid index in wxListBox::SetFirstItem") );

    SendMessage(GetHwnd(), LB_SETTOPINDEX, (WPARAM)N, (LPARAM)0);
}

void wxListBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( IsValid(n),
                 wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    if ( HasFlag(wxLB_OWNERDRAW) )
    {
        delete m_aItems[n];
        m_aItems.RemoveAt(n);
    }
#endif // wxUSE_OWNER_DRAWN

    SendMessage(GetHwnd(), LB_DELETESTRING, n, 0);
    m_noItems--;

    MSWOnItemsChanged();

    UpdateOldSelections();
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    // back to base class search for not native search type
    if (bCase)
       return wxItemContainerImmutable::FindString( s, bCase );

    int pos = ListBox_FindStringExact(GetHwnd(), -1, s.wx_str());
    if (pos == LB_ERR)
        return wxNOT_FOUND;
    else
        return pos;
}

void wxListBox::DoClear()
{
#if wxUSE_OWNER_DRAWN
    if ( HasFlag(wxLB_OWNERDRAW) )
    {
        WX_CLEAR_ARRAY(m_aItems);
    }
#endif // wxUSE_OWNER_DRAWN

    ListBox_ResetContent(GetHwnd());

    m_noItems = 0;
    MSWOnItemsChanged();

    UpdateOldSelections();
}

void wxListBox::DoSetSelection(int N, bool select)
{
    wxCHECK_RET( N == wxNOT_FOUND || IsValid(N),
                 wxT("invalid index in wxListBox::SetSelection") );

    if ( HasMultipleSelection() )
    {
        SendMessage(GetHwnd(), LB_SETSEL, select, N);
    }
    else
    {
        SendMessage(GetHwnd(), LB_SETCURSEL, select ? N : -1, 0);
    }

    UpdateOldSelections();
}

bool wxListBox::IsSelected(int N) const
{
    wxCHECK_MSG( IsValid(N), false,
                 wxT("invalid index in wxListBox::Selected") );

    return SendMessage(GetHwnd(), LB_GETSEL, N, 0) == 0 ? false : true;
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), NULL,
                 wxT("invalid index in wxListBox::GetClientData") );

    return (void *)SendMessage(GetHwnd(), LB_GETITEMDATA, n, 0);
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    wxCHECK_RET( IsValid(n),
                 wxT("invalid index in wxListBox::SetClientData") );

    if ( ListBox_SetItemData(GetHwnd(), n, clientData) == LB_ERR )
    {
        wxLogDebug(wxT("LB_SETITEMDATA failed"));
    }
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

    if ( HasMultipleSelection() )
    {
        int countSel = ListBox_GetSelCount(GetHwnd());
        if ( countSel == LB_ERR )
        {
            wxLogDebug(wxT("ListBox_GetSelCount failed"));
        }
        else if ( countSel != 0 )
        {
            int *selections = new int[countSel];

            if ( ListBox_GetSelItems(GetHwnd(),
                                     countSel, selections) == LB_ERR )
            {
                wxLogDebug(wxT("ListBox_GetSelItems failed"));
                countSel = -1;
            }
            else
            {
                aSelections.Alloc(countSel);
                for ( int n = 0; n < countSel; n++ )
                    aSelections.Add(selections[n]);
            }

            delete [] selections;
        }

        return countSel;
    }
    else  // single-selection listbox
    {
        if (ListBox_GetCurSel(GetHwnd()) > -1)
            aSelections.Add(ListBox_GetCurSel(GetHwnd()));

        return aSelections.Count();
    }
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(),
                 -1,
                 wxT("GetSelection() can't be used with multiple-selection listboxes, use GetSelections() instead.") );

    return ListBox_GetCurSel(GetHwnd());
}

// Find string for position
wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString,
                 wxT("invalid index in wxListBox::GetString") );

    int len = ListBox_GetTextLen(GetHwnd(), n);

    // +1 for terminating NUL
    wxString result;
    ListBox_GetText(GetHwnd(), n, (wxChar*)wxStringBuffer(result, len + 1));

    return result;
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                             unsigned int pos,
                             void **clientData,
                             wxClientDataType type)
{
    MSWAllocStorage(items, LB_INITSTORAGE);

    const bool append = pos == GetCount();

    // we must use CB_ADDSTRING when appending as only it works correctly for
    // the sorted controls
    const unsigned msg = append ? LB_ADDSTRING : LB_INSERTSTRING;

    if ( append )
        pos = 0;

    int n = wxNOT_FOUND;

    const unsigned int numItems = items.GetCount();
    for ( unsigned int i = 0; i < numItems; i++ )
    {
        n = MSWInsertOrAppendItem(pos, items[i], msg);
        if ( n == wxNOT_FOUND )
            return n;

        if ( !append )
            pos++;

        ++m_noItems;

#if wxUSE_OWNER_DRAWN
        if ( HasFlag(wxLB_OWNERDRAW) )
        {
            wxOwnerDrawn *pNewItem = CreateLboxItem(n);
            pNewItem->SetFont(GetFont());
            m_aItems.Insert(pNewItem, n);
        }
#endif // wxUSE_OWNER_DRAWN
        AssignNewItemClientData(n, clientData, i, type);
    }

    MSWOnItemsChanged();

    UpdateOldSelections();

    return n;
}

int wxListBox::DoHitTestList(const wxPoint& point) const
{
    LRESULT lRes = ::SendMessage(GetHwnd(), LB_ITEMFROMPOINT,
                                 0, MAKELPARAM(point.x, point.y));

    // non zero high-order word means that this item is outside of the client
    // area, IOW the point is outside of the listbox
    return HIWORD(lRes) ? wxNOT_FOUND : LOWORD(lRes);
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( IsValid(n),
                 wxT("invalid index in wxListBox::SetString") );

    // remember the state of the item
    bool wasSelected = IsSelected(n);

    void *oldData = NULL;
    wxClientData *oldObjData = NULL;
    if ( HasClientUntypedData() )
        oldData = GetClientData(n);
    else if ( HasClientObjectData() )
        oldObjData = GetClientObject(n);

    // delete and recreate it
    SendMessage(GetHwnd(), LB_DELETESTRING, n, 0);

    int newN = n;
    if ( n == (m_noItems - 1) )
        newN = -1;

    ListBox_InsertString(GetHwnd(), newN, s.wx_str());

    // restore the client data
    if ( oldData )
        SetClientData(n, oldData);
    else if ( oldObjData )
        SetClientObject(n, oldObjData);

    // we may have lost the selection
    if ( wasSelected )
        Select(n);

    MSWOnItemsChanged();
}

unsigned int wxListBox::GetCount() const
{
    return m_noItems;
}

// ----------------------------------------------------------------------------
// size-related stuff
// ----------------------------------------------------------------------------

void wxListBox::SetHorizontalExtent(const wxString& s)
{
    // the rest is only necessary if we want a horizontal scrollbar
    if ( !HasFlag(wxHSCROLL) )
        return;


    WindowHDC dc(GetHwnd());
    SelectInHDC selFont(dc, GetHfontOf(GetFont()));

    TEXTMETRIC lpTextMetric;
    ::GetTextMetrics(dc, &lpTextMetric);

    int largestExtent = 0;
    SIZE extentXY;

    if ( s.empty() )
    {
        // set extent to the max length of all strings
        for ( unsigned int i = 0; i < m_noItems; i++ )
        {
            const wxString str = GetString(i);
            ::GetTextExtentPoint32(dc, str.c_str(), str.length(), &extentXY);

            int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);
            if ( extentX > largestExtent )
                largestExtent = extentX;
        }
    }
    else // just increase the extent to the length of this string
    {
        int existingExtent = (int)SendMessage(GetHwnd(),
                                              LB_GETHORIZONTALEXTENT, 0, 0L);

        ::GetTextExtentPoint32(dc, s.c_str(), s.length(), &extentXY);

        int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);
        if ( extentX > existingExtent )
            largestExtent = extentX;
    }

    if ( largestExtent )
        SendMessage(GetHwnd(), LB_SETHORIZONTALEXTENT, LOWORD(largestExtent), 0L);
    //else: it shouldn't change
}

wxSize wxListBox::DoGetBestClientSize() const
{
    // find the widest string
    int wLine;
    int wListbox = 0;
    for (unsigned int i = 0; i < m_noItems; i++)
    {
        wxString str(GetString(i));
        GetTextExtent(str, &wLine, NULL);
        if ( wLine > wListbox )
            wListbox = wLine;
    }

    // give it some reasonable default value if there are no strings in the
    // list
    if ( wListbox == 0 )
        wListbox = 100;

    // the listbox should be slightly larger than the widest string
    wListbox += 3*GetCharWidth();

    // add room for the scrollbar
    wListbox += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // don't make the listbox too tall (limit height to 10 items) but don't
    // make it too small neither
    int hListbox = SendMessage(GetHwnd(), LB_GETITEMHEIGHT, 0, 0)*
                    wxMin(wxMax(m_noItems, 3), 10);

    return wxSize(wListbox, hListbox);
}

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

bool wxListBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    wxEventType evtType;
    int n = wxNOT_FOUND;
    if ( param == LBN_SELCHANGE )
    {
        if ( HasMultipleSelection() )
            return CalcAndSendEvent();

        evtType = wxEVT_COMMAND_LISTBOX_SELECTED;

        if ( m_selectedByKeyboard )
        {
            // We shouldn't use the mouse position to find the item as mouse
            // can be anywhere, ask the listbox itself. Notice that this can't
            // be used when the item is selected using the mouse however as
            // LB_GETCARETINDEX will always return a valid item, even if the
            // mouse is clicked below all the items, which is why we find the
            // item ourselves below in this case.
            n = SendMessage(GetHwnd(), LB_GETCARETINDEX, 0, 0);
        }
        //else: n will be determined below from the mouse position
    }
    else if ( param == LBN_DBLCLK )
    {
        evtType = wxEVT_COMMAND_LISTBOX_DOUBLECLICKED;
    }
    else
    {
        // some event we're not interested in
        return false;
    }

    // Find the item position if it was a mouse-generated selection event or a
    // double click event (which is always generated using the mouse)
    if ( n == wxNOT_FOUND )
    {
        const DWORD pos = ::GetMessagePos();
        const wxPoint pt(GET_X_LPARAM(pos), GET_Y_LPARAM(pos));
        n = HitTest(ScreenToClient(wxPoint(pt)));
    }

    // We get events even when mouse is clicked outside of any valid item from
    // Windows, just ignore them.
    if ( n == wxNOT_FOUND )
       return false;

    // As we don't use m_oldSelections in single selection mode, we store the
    // last item that we notified the user about in it in this case because we
    // need to remember it to be able to filter out the dummy LBN_SELCHANGE
    // messages that we get when the user clicks on an already selected item.
    if ( param == LBN_SELCHANGE )
    {
        if ( !m_oldSelections.empty() && *m_oldSelections.begin() == n )
        {
            // Same item as the last time.
            return false;
        }

        m_oldSelections.clear();
        m_oldSelections.push_back(n);
    }

    // Do generate an event otherwise.
    return SendEvent(evtType, n, true /* selection */);
}

WXLRESULT
wxListBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // Remember whether there was a keyboard or mouse event before
    // LBN_SELCHANGE: this allows us to correctly determine the item affected
    // by it in MSWCommand() above in any case.
    if ( WM_KEYFIRST <= nMsg && nMsg <= WM_KEYLAST )
        m_selectedByKeyboard = true;
    else if ( WM_MOUSEFIRST <= nMsg && nMsg <= WM_MOUSELAST )
        m_selectedByKeyboard = false;

    return wxListBoxBase::MSWWindowProc(nMsg, wParam, lParam);
}

// ----------------------------------------------------------------------------
// owner-drawn list boxes support
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

// misc overloaded methods
// -----------------------

bool wxListBox::SetFont(const wxFont &font)
{
    if ( HasFlag(wxLB_OWNERDRAW) )
    {
        const unsigned count = m_aItems.GetCount();
        for ( unsigned i = 0; i < count; i++ )
            m_aItems[i]->SetFont(font);
    }

    wxListBoxBase::SetFont(font);

    return true;
}

bool wxListBox::GetItemRect(size_t n, wxRect& rect) const
{
    wxCHECK_MSG( IsValid(n), false,
                 wxT("invalid index in wxListBox::GetItemRect") );

    RECT rc;

    if ( ListBox_GetItemRect(GetHwnd(), n, &rc) != LB_ERR )
    {
        rect = wxRectFromRECT(rc);
        return true;
    }
    else
    {
        // couldn't retrieve rect: for example, item isn't visible
        return false;
    }
}

bool wxListBox::RefreshItem(size_t n)
{
    wxRect rect;
    if ( !GetItemRect(n, rect) )
        return false;

    RECT rc;
    wxCopyRectToRECT(rect, rc);

    return ::InvalidateRect((HWND)GetHWND(), &rc, FALSE) == TRUE;
}


// drawing
// -------

namespace
{
    // space beneath/above each row in pixels
    static const int LISTBOX_EXTRA_SPACE = 1;

} // anonymous namespace

// the height is the same for all items
// TODO should be changed for LBS_OWNERDRAWVARIABLE style listboxes

// NB: can't forward this to wxListBoxItem because LB_SETITEMDATA
//     message is not yet sent when we get here!
bool wxListBox::MSWOnMeasure(WXMEASUREITEMSTRUCT *item)
{
    // only owner-drawn control should receive this message
    wxCHECK( HasFlag(wxLB_OWNERDRAW), false );

    MEASUREITEMSTRUCT *pStruct = (MEASUREITEMSTRUCT *)item;

#ifdef __WXWINCE__
    HDC hdc = GetDC(NULL);
#else
    HDC hdc = CreateIC(wxT("DISPLAY"), NULL, NULL, 0);
#endif

    {
        wxDCTemp dc((WXHDC)hdc);
        dc.SetFont(GetFont());

        pStruct->itemHeight = dc.GetCharHeight() + 2 * LISTBOX_EXTRA_SPACE;
        pStruct->itemWidth  = dc.GetCharWidth();
    }

#ifdef __WXWINCE__
    ReleaseDC(NULL, hdc);
#else
    DeleteDC(hdc);
#endif

    return true;
}

// forward the message to the appropriate item
bool wxListBox::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
    // only owner-drawn control should receive this message
    wxCHECK( HasFlag(wxLB_OWNERDRAW), false );

    DRAWITEMSTRUCT *pStruct = (DRAWITEMSTRUCT *)item;

    // the item may be -1 for an empty listbox
    if ( pStruct->itemID == (UINT)-1 )
        return false;

    wxListBoxItem *pItem = (wxListBoxItem *)m_aItems[pStruct->itemID];

    wxDCTemp dc((WXHDC)pStruct->hDC);

    return pItem->OnDrawItem(dc, wxRectFromRECT(pStruct->rcItem),
                             (wxOwnerDrawn::wxODAction)pStruct->itemAction,
                             (wxOwnerDrawn::wxODStatus)(pStruct->itemState | wxOwnerDrawn::wxODHidePrefix));
}

#endif // wxUSE_OWNER_DRAWN

#endif // wxUSE_LISTBOX
