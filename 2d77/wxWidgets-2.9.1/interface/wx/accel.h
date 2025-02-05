/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     interface of wxAccelerator* classes
// Author:      wxWidgets team
// RCS-ID:      $Id: accel.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/** wxAcceleratorEntry flags */
enum wxAcceleratorEntryFlags
{
    /** no modifiers */
    wxACCEL_NORMAL,

    /** hold Alt key down */
    wxACCEL_ALT,

    /** hold Ctrl key down */
    wxACCEL_CTRL,

    /** hold Shift key down */
    wxACCEL_SHIFT,

    /** Command key on OS X; identic to wxACCEL_CTRL on other platforms. */
    wxACCEL_CMD
};


/**
    @class wxAcceleratorEntry

    An object used by an application wishing to create an accelerator table
    (see wxAcceleratorTable).

    @library{wxcore}
    @category{data}

    @see wxAcceleratorTable, wxWindow::SetAcceleratorTable
*/
class wxAcceleratorEntry
{
public:
    /**
        Constructor.

        @param flags
            A combination of the ::wxAcceleratorEntryFlags values, which
            indicates which modifier keys are held down.
        @param keyCode
            The keycode to be detected. See ::wxKeyCode for a full list of keycodes.
        @param cmd
            The menu or control command identifier (ID).
        @param item
            The menu item associated with this accelerator.
    */
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0,
                       wxMenuItem *item = NULL);

    /**
        Copy ctor.
    */
    wxAcceleratorEntry(const wxAcceleratorEntry& entry);

    /**
        Returns the command identifier for the accelerator table entry.
    */
    int GetCommand() const;

    /**
        Returns the flags for the accelerator table entry.
    */
    int GetFlags() const;

    /**
        Returns the keycode for the accelerator table entry.
    */
    int GetKeyCode() const;

    /**
        Returns the menu item associated with this accelerator entry.
    */
    wxMenuItem *GetMenuItem() const;

    /**
        Sets the accelerator entry parameters.

        @param flags
            A combination of the ::wxAcceleratorEntryFlags values, which
            indicates which modifier keys are held down.
        @param keyCode
            The keycode to be detected. See ::wxKeyCode for a full list of keycodes.
        @param cmd
            The menu or control command identifier (ID).
        @param item
            The menu item associated with this accelerator.
    */
    void Set(int flags, int keyCode, int cmd, wxMenuItem *item = NULL);

    /**
        Returns @true if this object is correctly initialized.
    */
    bool IsOk() const;

    /**
        Returns a wxString for this accelerator.

        This function formats it using the @c "flags-keycode" format
        where @c flags maybe a hyphen-separed list of @c "shift|alt|ctrl".
    */
    wxString ToString() const;

    /**
        Parses the given string and sets the accelerator accordingly.

        @param str
            Should be a string in the form "flags-keycode"

        @return @true if the given string correctly initialized this object
                (i.e. if IsOk() returns true after this call)
    */
    bool FromString(const wxString& str);


    wxAcceleratorEntry& operator=(const wxAcceleratorEntry& entry);
    bool operator==(const wxAcceleratorEntry& entry) const;
    bool operator!=(const wxAcceleratorEntry& entry) const;
};


/**
    @class wxAcceleratorTable

    An accelerator table allows the application to specify a table of keyboard
    shortcuts for menu or button commands.

    The object ::wxNullAcceleratorTable is defined to be a table with no data, and
    is the initial accelerator table for a window.

    Example:

    @code
    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', ID_NEW_WINDOW);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', wxID_EXIT);
    entries[2].Set(wxACCEL_SHIFT, (int) 'A', ID_ABOUT);
    entries[3].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_CUT);

    wxAcceleratorTable accel(4, entries);
    frame->SetAcceleratorTable(accel);
    @endcode

    @remarks
    An accelerator takes precedence over normal processing and can be a convenient
    way to program some event handling. For example, you can use an accelerator table
    to enable a dialog with a multi-line text control to accept CTRL-Enter as meaning
    'OK'.

    @library{wxcore}
    @category{data}

    @stdobjects
    ::wxNullAcceleratorTable

    @see wxAcceleratorEntry, wxWindow::SetAcceleratorTable
*/
class wxAcceleratorTable : public wxObject
{
public:
    /**
        Default ctor.
    */
    wxAcceleratorTable();

    /**
        Initializes the accelerator table from an array of wxAcceleratorEntry.

        @param n
            Number of accelerator entries.
        @param entries
            The array of entries.

        @beginWxPerlOnly
        The wxPerl constructor accepts a list of either
        Wx::AcceleratorEntry objects or references to 3-element arrays
        [flags, keyCode, cmd] , like the parameters of
        Wx::AcceleratorEntry::new.
        @endWxPerlOnly
    */
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    /**
        Loads the accelerator table from a Windows resource (Windows only).

        @onlyfor{wxmsw}

        @param resource
            Name of a Windows accelerator.
    */
    wxAcceleratorTable(const wxString& resource);

    /**
        Destroys the wxAcceleratorTable object.
        See @ref overview_refcount_destruct for more info.
    */
    virtual ~wxAcceleratorTable();

    /**
        Returns @true if the accelerator table is valid.
    */
    bool IsOk() const;
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    An empty accelerator table.
*/
wxAcceleratorTable wxNullAcceleratorTable;
