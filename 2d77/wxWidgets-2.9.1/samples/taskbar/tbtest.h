/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.h
// Purpose:     wxTaskBarIcon sample
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: tbtest.h 50402 2007-12-01 16:25:30Z VZ $
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

class MyTaskBarIcon : public wxTaskBarIcon
{
public:
#if defined(__WXCOCOA__)
    MyTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    MyTaskBarIcon()
#endif
    {}

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuCheckmark(wxCommandEvent&);
    void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
    virtual wxMenu *CreatePopupMenu();

    DECLARE_EVENT_TABLE()
};


// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyDialog: public wxDialog
{
public:
    MyDialog(const wxString& title);
    virtual ~MyDialog();

protected:
    void OnAbout(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXCOCOA__)
    MyTaskBarIcon   *m_dockIcon;
#endif

    DECLARE_EVENT_TABLE()
};
