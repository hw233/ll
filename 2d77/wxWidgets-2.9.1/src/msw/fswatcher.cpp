/////////////////////////////////////////////////////////////////////////////
// Name:        msw/fswatcher.cpp
// Purpose:     wxMSWFileSystemWatcher
// Author:      Bartosz Bekier
// Created:     2009-05-26
// RCS-ID:      $Id: fswatcher.cpp 63166 2010-01-18 00:28:15Z VZ $
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FSWATCHER

#include "wx/fswatcher.h"
#include "wx/thread.h"
#include "wx/sharedptr.h"
#include "wx/msw/fswatcher.h"
#include "wx/msw/private.h"
#include "wx/private/fswatcher.h"

// ============================================================================
// wxFSWatcherImplMSW implementation
// ============================================================================

class wxFSWatcherImplMSW : public wxFSWatcherImpl
{
public:
    wxFSWatcherImplMSW(wxFileSystemWatcherBase* watcher);

    virtual ~wxFSWatcherImplMSW();

    bool SetUpWatch(wxFSWatchEntryMSW& watch);

    void SendEvent(wxFileSystemWatcherEvent& evt);

protected:
    bool Init();

    // adds watch to be monitored for file system changes
    virtual bool DoAdd(wxSharedPtr<wxFSWatchEntryMSW> watch);

    virtual bool DoRemove(wxSharedPtr<wxFSWatchEntryMSW> watch);

private:
    bool DoSetUpWatch(wxFSWatchEntryMSW& watch);

    static int Watcher2NativeFlags(int flags);

    wxIOCPService m_iocp;
    wxIOCPThread m_workerThread;
};

wxFSWatcherImplMSW::wxFSWatcherImplMSW(wxFileSystemWatcherBase* watcher) :
    wxFSWatcherImpl(watcher),
    m_workerThread(this, &m_iocp)
{
}

wxFSWatcherImplMSW::~wxFSWatcherImplMSW()
{
    // order the worker thread to finish & wait
    m_workerThread.Finish();
    if (m_workerThread.Wait() != 0)
    {
        wxLogError(_("Ungraceful worker thread termination"));
    }

    // remove all watches
    (void) RemoveAll();
}

bool wxFSWatcherImplMSW::Init()
{
    wxCHECK_MSG( !m_workerThread.IsAlive(), false,
                 "Watcher service is already initialized" );

    if (m_workerThread.Create() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Unable to create IOCP worker thread"));
        return false;
    }

    // we have valid iocp service and thread
    if (m_workerThread.Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Unable to start IOCP worker thread"));
        return false;
    }

    return true;
}

// adds watch to be monitored for file system changes
bool wxFSWatcherImplMSW::DoAdd(wxSharedPtr<wxFSWatchEntryMSW> watch)
{
    // setting up wait for directory changes
    if (!DoSetUpWatch(*watch))
        return false;

    // associating handle with completion port
    return m_iocp.Add(watch);
}

bool
wxFSWatcherImplMSW::DoRemove(wxSharedPtr<wxFSWatchEntryMSW> WXUNUSED(watch))
{
    return true;
}

// TODO ensuring that we have not already set watch for this handle/dir?
bool wxFSWatcherImplMSW::SetUpWatch(wxFSWatchEntryMSW& watch)
{
    wxCHECK_MSG( watch.IsOk(), false, "Invalid watch" );
    if (m_watches.find(watch.GetPath()) == m_watches.end())
    {
        wxLogTrace(wxTRACE_FSWATCHER, "Path '%s' is not watched",
                   watch.GetPath());
        return false;
    }

    wxLogTrace(wxTRACE_FSWATCHER, "Setting up watch for file system changes...");
    return DoSetUpWatch(watch);
}

void wxFSWatcherImplMSW::SendEvent(wxFileSystemWatcherEvent& evt)
{
    // called from worker thread, so posting event in thread-safe way
    wxQueueEvent(m_watcher->GetOwner(), evt.Clone());
}

bool wxFSWatcherImplMSW::DoSetUpWatch(wxFSWatchEntryMSW& watch)
{
    int flags = Watcher2NativeFlags(watch.GetFlags());
    int ret = ReadDirectoryChangesW(watch.GetHandle(), watch.GetBuffer(),
                                    wxFSWatchEntryMSW::BUFFER_SIZE, TRUE,
                                    flags, NULL,
                                    watch.GetOverlapped(), NULL);
    if (!ret)
    {
        wxLogSysError(_("Unable to set up watch for '%s'"),
                        watch.GetPath());
    }

    return ret != 0;
}

// TODO we should only specify those flags, which interest us
// this needs a bit of thinking, quick impl for now
int wxFSWatcherImplMSW::Watcher2NativeFlags(int WXUNUSED(flags))
{
    static DWORD all_events = FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_SECURITY;

    return all_events;
}


// ============================================================================
// wxFSWatcherImplMSW helper classes implementation
// ============================================================================

wxIOCPThread::wxIOCPThread(wxFSWatcherImplMSW* service, wxIOCPService* iocp) :
    wxThread(wxTHREAD_JOINABLE),
    m_service(service), m_iocp(iocp)
{
}

// finishes this thread
bool wxIOCPThread::Finish()
{
    wxLogTrace(wxTRACE_FSWATCHER, "Posting empty status!");

    // send "empty" packet to i/o completion port, just to wake
    return m_iocp->PostEmptyStatus();
}

wxThread::ExitCode wxIOCPThread::Entry()
{
    wxLogTrace(wxTRACE_FSWATCHER, "[iocp] Started IOCP thread");

    // read events in a loop until we get false, which means we should exit
    while ( ReadEvents() );

    wxLogTrace(wxTRACE_FSWATCHER, "[iocp] Ended IOCP thread");
    return (ExitCode)0;
}

// wait for events to occur, read them and send to interested parties
// returns false it empty status was read, which means we whould exit
//         true otherwise
bool wxIOCPThread::ReadEvents()
{
    unsigned long count = 0;
    wxFSWatchEntryMSW* watch = NULL;
    OVERLAPPED* overlapped = NULL;
    if (!m_iocp->GetStatus(&count, &watch, &overlapped))
        return true; // error was logged already, we don't want to exit

    // this is our exit condition, so we return false
    if (!count && !watch && !overlapped)
        return false;

    // in case of spurious wakeup
    if (!count || !watch)
        return true;

    wxLogTrace( wxTRACE_FSWATCHER, "[iocp] Read entry: path='%s'",
                watch->GetPath());

    // extract events from buffer info our vector container
    wxVector<wxEventProcessingData> events;
    const char* memory = static_cast<const char*>(watch->GetBuffer());
    int offset = 0;
    do
    {
        const FILE_NOTIFY_INFORMATION* e =
              static_cast<const FILE_NOTIFY_INFORMATION*>((const void*)memory);

        events.push_back(wxEventProcessingData(e, watch));

        offset = e->NextEntryOffset;
        memory += offset;
    }
    while (offset);

    // process events
    ProcessNativeEvents(events);

    // reissue the watch. ignore possible errors, we will return true anyway
    (void) m_service->SetUpWatch(*watch);

    return true;
}

void wxIOCPThread::ProcessNativeEvents(wxVector<wxEventProcessingData>& events)
{
    wxVector<wxEventProcessingData>::iterator it = events.begin();
    for ( ; it != events.end(); ++it )
    {
        const FILE_NOTIFY_INFORMATION& e = *(it->nativeEvent);
        const wxFSWatchEntryMSW* watch = it->watch;

        wxLogTrace( wxTRACE_FSWATCHER, "[iocp] %s",
                    FileNotifyInformationToString(e));

        int nativeFlags = e.Action;
        int flags = Native2WatcherFlags(nativeFlags);
        if (flags & wxFSW_EVENT_WARNING || flags & wxFSW_EVENT_ERROR)
        {
            // TODO think about this...do we ever have any errors to report?
            wxString errMsg = "Error occurred";
            wxFileSystemWatcherEvent event(flags, errMsg);
            SendEvent(event);
        }
        // filter out ignored events and those not asked for.
        // we never filter out warnings or exceptions
        else if ((flags == 0) || !(flags & watch->GetFlags()))
        {
            return;
        }
        // rename case
        else if (nativeFlags == FILE_ACTION_RENAMED_OLD_NAME)
        {
            wxFileName oldpath = GetEventPath(*watch, e);
            wxFileName newpath;

            // newpath should be in the next entry. what if there isn't?
            ++it;
            if ( it != events.end() )
            {
                newpath = GetEventPath(*(it->watch), *(it->nativeEvent));
            }
            wxFileSystemWatcherEvent event(flags, oldpath, newpath);
            SendEvent(event);
        }
        // all other events
        else
        {
            // CHECK I heard that returned path can be either in short on long
            // form...need to account for that!
            wxFileName path = GetEventPath(*watch, e);
            wxFileSystemWatcherEvent event(flags, path, path);
            SendEvent(event);
        }
    }
}

void wxIOCPThread::SendEvent(wxFileSystemWatcherEvent& evt)
{
    wxLogTrace(wxTRACE_FSWATCHER, "[iocp] EVT: %s", evt.ToString());
    m_service->SendEvent(evt);
}

int wxIOCPThread::Native2WatcherFlags(int flags)
{
    static const int flag_mapping[][2] = {
        { FILE_ACTION_ADDED,            wxFSW_EVENT_CREATE },
        { FILE_ACTION_REMOVED,          wxFSW_EVENT_DELETE },

        // TODO take attributes into account to see what happened
        { FILE_ACTION_MODIFIED,         wxFSW_EVENT_MODIFY },

        { FILE_ACTION_RENAMED_OLD_NAME, wxFSW_EVENT_RENAME },

        // ignored as it should always be matched with ***_OLD_NAME
        { FILE_ACTION_RENAMED_NEW_NAME, 0 },
    };

    for (unsigned int i=0; i < WXSIZEOF(flag_mapping); ++i) {
        if (flags == flag_mapping[i][0])
            return flag_mapping[i][1];
    }

    // never reached
    wxFAIL_MSG(wxString::Format("Unknown file notify change %u", flags));
    return -1;
}

wxString wxIOCPThread::FileNotifyInformationToString(
                                              const FILE_NOTIFY_INFORMATION& e)
{
    wxString fname(e.FileName, e.FileNameLength / sizeof(e.FileName[0]));
    return wxString::Format("Event: offset=%d, action=%d, len=%d, "
                            "name(approx)='%s'", e.NextEntryOffset, e.Action,
                            e.FileNameLength, fname);
}

wxFileName wxIOCPThread::GetEventPath(const wxFSWatchEntryMSW& watch,
                                      const FILE_NOTIFY_INFORMATION& e)
{
    wxFileName path = watch.GetPath();
    if (path.IsDir())
    {
        wxString rel(e.FileName, e.FileNameLength / sizeof(e.FileName[0]));
        int pathFlags = wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR;
        path = wxFileName(path.GetPath(pathFlags) + rel);
    }
    return path;
}


// ============================================================================
// wxMSWFileSystemWatcher implementation
// ============================================================================

wxMSWFileSystemWatcher::wxMSWFileSystemWatcher() :
    wxFileSystemWatcherBase()
{
    (void) Init();
}

wxMSWFileSystemWatcher::wxMSWFileSystemWatcher(const wxFileName& path,
                                               int events) :
    wxFileSystemWatcherBase()
{
    if (!Init())
        return;

    Add(path, events);
}

bool wxMSWFileSystemWatcher::Init()
{
    m_service = new wxFSWatcherImplMSW(this);
    bool ret = m_service->Init();
    if (!ret)
    {
        delete m_service;
    }

    return ret;
}

#endif // wxUSE_FSWATCHER
