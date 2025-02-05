/////////////////////////////////////////////////////////////////////////////
// Name:        aboutdlg.h
// Purpose:     interface of wxAboutDialogInfo
// Author:      wxWidgets team
// RCS-ID:      $Id: aboutdlg.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxAboutDialogInfo

    wxAboutDialogInfo contains information shown in the standard @e About
    dialog displayed by the wxAboutBox() function.

    This class contains the general information about the program, such as its
    name, version, copyright and so on, as well as lists of the program developers,
    documentation writers, artists and translators. The simple properties from the
    former group are represented as a string with the exception of the program icon
    and the program web site, while the lists from the latter group are stored as
    wxArrayString and can be either set entirely at once using
    wxAboutDialogInfo::SetDevelopers and similar functions or built one by one using
    wxAboutDialogInfo::AddDeveloper etc.

    Please also notice that while all the main platforms have the native
    implementation of the about dialog, they are often more limited than the
    generic version provided by wxWidgets and so the generic version is used if
    wxAboutDialogInfo has any fields not supported by the native version. Currently
    GTK+ version supports all the possible fields natively but MSW and Mac versions
    don't support URLs, licence text nor custom icons in the about dialog and if
    either of those is used, wxAboutBox() will automatically use the generic version
    so you should avoid specifying these fields to achieve more native look and feel.
    
    Example of usage:
    @code
    void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
    {
        wxAboutDialogInfo aboutInfo;
        aboutInfo.SetName("MyApp");
        aboutInfo.SetVersion(MY_APP_VERSION_STRING);
        aboutInfo.SetDescription(_("My wxWidgets-based application!"));
        aboutInfo.SetCopyright("(C) 1992-2010");
        aboutInfo.SetWebSite("http://myapp.org");
        aboutInfo.AddDeveloper("My Self");

        wxAboutBox(aboutInfo);
    }
    @endcode

    @library{wxadv}
    @category{cmndlg,data}

    @see wxAboutDialogInfo::SetArtists
*/
class wxAboutDialogInfo
{
public:
    /**
        Default constructor leaves all fields are initially uninitialized, in general
        you should call at least SetVersion(), SetCopyright() and SetDescription().
    */
    wxAboutDialogInfo();

    /**
        Adds an artist name to be shown in the program credits.

        @see SetArtists()
    */
    void AddArtist(const wxString& artist);

    /**
        Adds a developer name to be shown in the program credits.

        @see SetDevelopers()
    */
    void AddDeveloper(const wxString& developer);

    /**
        Adds a documentation writer name to be shown in the program credits.

        @see SetDocWriters()
    */
    void AddDocWriter(const wxString& docwriter);

    /**
        Adds a translator name to be shown in the program credits. Notice that if no
        translator names are specified explicitely, wxAboutBox() will try to use the
        translation of the string @c translator-credits from the currently used message
        catalog -- this can be used to show just the name of the translator of the
        program in the current language.

        @see SetTranslators()
    */
    void AddTranslator(const wxString& translator);

    /**
        Sets the the list of artists to be shown in the program credits.

        @see AddArtist()
    */
    void SetArtists(const wxArrayString& artists);

    /**
        Set the short string containing the program copyright information. Notice that
        any occurrences of @c "(C)" in @a copyright will be replaced by the
        copyright symbol (circled C) automatically, which means that you can avoid
        using this symbol in the program source code which can be problematic,
    */
    void SetCopyright(const wxString& copyright);

    /**
        Set brief, but possibly multiline, description of the program.
    */
    void SetDescription(const wxString& desc);

    /**
        Set the list of developers of the program.

        @see AddDeveloper()
    */
    void SetDevelopers(const wxArrayString& developers);

    /**
        Set the list of documentation writers.

        @see AddDocWriter()
    */
    void SetDocWriters(const wxArrayString& docwriters);

    /**
        Set the icon to be shown in the dialog. By default the icon of the main frame
        will be shown if the native about dialog supports custom icons. If it doesn't
        but a valid icon is specified using this method, the generic about dialog is
        used instead so you should avoid calling this function for maximally native
        look and feel.
    */
    void SetIcon(const wxIcon& icon);

    /**
        Set the long, multiline string containing the text of the program licence.

        Only GTK+ version supports showing the licence text in the native about dialog
        currently so the generic version will be used under all the other platforms if
        this method is called. To preserve the native look and feel it is advised that
        you do not call this method but provide a separate menu item in the
        @c "Help" menu for displaying the text of your program licence.
    */
    void SetLicence(const wxString& licence);

    /**
        This is the same as SetLicence().
    */
    void SetLicense(const wxString& licence);

    /**
        Set the name of the program. If this method is not called, the string returned
        by wxApp::GetAppName will be shown in the dialog.
    */
    void SetName(const wxString& name);

    /**
        Set the list of translators. Please see AddTranslator() for additional
        discussion.
    */
    void SetTranslators(const wxArrayString& translators);

    /**
        Set the version of the program. The word "version" shouldn't be included
        in @a version. Example @a version values: "1.2" and "RC2". In about dialogs
        with more space set aside for version information, @a longVersion is used.
        Example @a longVersion values: "Version 1.2" and "Release Candidate 2".
        If @a version is non-empty but @a longVersion is empty, a long version
        is constructed automatically, using @a version (by simply prepending
        "Version " to @a version).

        The generic about dialog and native GTK+ dialog use @a version only,
        as a suffix to the program name. The native MSW and OS X about dialogs
        use the long version.
    */
    void SetVersion(const wxString& version, const wxString& longVersion = wxString());

    /**
        Set the web site for the program and its description (which defaults to @a url
        itself if empty).

        Please notice that only GTK+ version currently supports showing the link in the
        native about dialog so if this method is called, the generic version will be
        used under all the other platforms.
    */
    void SetWebSite(const wxString& url,
                    const wxString& desc = wxEmptyString);
};


// ============================================================================
// Global functions/macros
// ============================================================================

/** @addtogroup group_funcmacro_dialog */
//@{

/**
    This function shows the standard about dialog containing the information
    specified in @a info. If the current platform has a native about dialog
    which is capable of showing all the fields in @a info, the native dialog is
    used, otherwise the function falls back to the generic wxWidgets version of
    the dialog, i.e. does the same thing as wxGenericAboutBox.

    Here is an example of how this function may be used:

    @code
    void MyFrame::ShowSimpleAboutDialog(wxCommandEvent& WXUNUSED(event))
    {
        wxAboutDialogInfo info;
        info.SetName(_("My Program"));
        info.SetVersion(_("1.2.3 Beta"));
        info.SetDescription(_("This program does something great."));
        info.SetCopyright(wxT("(C) 2007 Me <my@email.addre.ss>"));

        wxAboutBox(info);
    }
    @endcode

    Please see the @ref page_samples_dialogs for more examples of using this
    function and wxAboutDialogInfo for the description of the information which
    can be shown in the about dialog.

    @header{wx/aboutdlg.h}
*/
void wxAboutBox(const wxAboutDialogInfo& info, wxWindow* parent = NULL);

/**
    This function does the same thing as wxAboutBox() except that it always uses
    the generic wxWidgets version of the dialog instead of the native one.

    This is mainly useful if you need to customize the dialog by e.g. adding
    custom controls to it (customizing the native dialog is not currently
    supported).

    See the @ref page_samples_dialogs for an example of about dialog
    customization.

    @see wxAboutDialogInfo

    @header{wx/aboutdlg.h}
*/
void wxGenericAboutBox(const wxAboutDialogInfo& info, wxWindow* parent = NULL);

//@}
