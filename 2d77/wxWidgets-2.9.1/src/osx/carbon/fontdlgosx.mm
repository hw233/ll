/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/fontdlgosx.cpp
// Purpose:     wxFontDialog class.
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-03
// RCS-ID:      $Id: fontdlgosx.mm 64068 2010-04-20 19:09:38Z SC $
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "wx/fontdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/cmndata.h"
#endif

#include "wx/fontutil.h"

// ============================================================================
// implementation
// ============================================================================


#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#if wxOSX_USE_EXPERIMENTAL_FONTDIALOG

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "wx/osx/private.h"

@interface wxMacFontPanelAccView : NSView
{
    BOOL m_okPressed ;
    BOOL m_shouldClose ;
    NSButton* m_cancelButton ;
    NSButton* m_okButton ;
}

- (IBAction)cancelPressed:(id)sender;
- (IBAction)okPressed:(id)sender;
- (void)resetFlags;
- (BOOL)closedWithOk;
- (BOOL)shouldCloseCarbon;
- (NSButton*)okButton;
@end

@implementation wxMacFontPanelAccView : NSView
- (id)initWithFrame:(NSRect)rectBox
{
    [super initWithFrame:rectBox];

    wxCFStringRef cfOkString( wxT("OK"), wxLocale::GetSystemEncoding() );
    wxCFStringRef cfCancelString( wxT("Cancel"), wxLocale::GetSystemEncoding() );

    NSRect rectCancel = NSMakeRect( (CGFloat) 10.0 , (CGFloat)10.0 , (CGFloat)82  , (CGFloat)24 );
    NSRect rectOK = NSMakeRect( (CGFloat)100.0 , (CGFloat)10.0 , (CGFloat)82  , (CGFloat)24 );

    NSButton* cancelButton = [[NSButton alloc] initWithFrame:rectCancel];
    [cancelButton setTitle:(NSString*)wxCFRetain((CFStringRef)cfCancelString)];
    [cancelButton setBezelStyle:NSRoundedBezelStyle];
    [cancelButton setButtonType:NSMomentaryPushInButton];
    [cancelButton setAction:@selector(cancelPressed:)];
    [cancelButton setTarget:self];
    m_cancelButton = cancelButton ;

    NSButton* okButton = [[NSButton alloc] initWithFrame:rectOK];
    [okButton setTitle:(NSString*)wxCFRetain((CFStringRef)cfOkString)];
    [okButton setBezelStyle:NSRoundedBezelStyle];
    [okButton setButtonType:NSMomentaryPushInButton];
    [okButton setAction:@selector(okPressed:)];
    [okButton setTarget:self];
    // doesn't help either, the button is not highlighted after a color dialog has been used
    // [okButton setKeyEquivalent:@"\r"];
    m_okButton = okButton ;


    [self addSubview:cancelButton];
    [self addSubview:okButton];

    [self resetFlags];
    return self;
}

- (void)resetFlags
{
    m_okPressed = NO ;
    m_shouldClose = NO ;
}

- (IBAction)cancelPressed:(id)sender
{
    wxUnusedVar(sender);
    m_shouldClose = YES ;
    [NSApp stopModal];
}

- (IBAction)okPressed:(id)sender
{
    wxUnusedVar(sender);
    m_okPressed = YES ;
    m_shouldClose = YES ;
    [NSApp stopModal];
}

-(BOOL)closedWithOk
{
    return m_okPressed ;
}

-(BOOL)shouldCloseCarbon
{
    return m_shouldClose ;
}

-(NSButton*)okButton
{
    return m_okButton ;
}
@end


extern "C" int RunMixedFontDialog(wxFontDialog* dialog) ;

int RunMixedFontDialog(wxFontDialog* dialog)
{
#if wxOSX_USE_COCOA
    wxFontData& fontdata= ((wxFontDialog*)dialog)->GetFontData() ;
#else
    wxUnusedVar(dialog);
#endif
    int retval = wxID_CANCEL ;

    wxAutoNSAutoreleasePool pool;

    // setting up the ok/cancel buttons
    NSFontPanel* fontPanel = [NSFontPanel sharedFontPanel] ;

    // adjust modality for carbon environment
#if wxOSX_USE_CARBON
    WindowRef carbonWindowRef = (WindowRef)[fontPanel windowRef] ;
    SetWindowModality(carbonWindowRef, kWindowModalityAppModal , 0) ;
    SetWindowGroup(carbonWindowRef , GetWindowGroupOfClass(kMovableModalWindowClass));
#endif

    [fontPanel setFloatingPanel:NO] ;
    [[fontPanel standardWindowButton:NSWindowCloseButton] setEnabled:NO] ;

    wxMacFontPanelAccView* accessoryView = (wxMacFontPanelAccView*) [fontPanel accessoryView] ;
    if ( accessoryView == nil)
    {
        NSRect rectBox = NSMakeRect( 0 , 0 , 192 , 40 );
        accessoryView = [[wxMacFontPanelAccView alloc] initWithFrame:rectBox];
        [fontPanel setAccessoryView:accessoryView];

        [fontPanel setDefaultButtonCell:[[accessoryView okButton] cell]] ;
    }

    [accessoryView resetFlags];
#if wxOSX_USE_COCOA
    wxFont font = *wxNORMAL_FONT ;
    if ( fontdata.m_initialFont.Ok() )
    {
        font = fontdata.m_initialFont ;
    }

    [[NSFontPanel sharedFontPanel] setPanelFont: font.OSXGetNSFont() isMultiple:NO];

    if(fontdata.m_fontColour.Ok())
        [[NSColorPanel sharedColorPanel] setColor:
            [NSColor colorWithCalibratedRed:fontdata.m_fontColour.Red() / 255.0
                                        green:fontdata.m_fontColour.Green() / 255.0
                                        blue:fontdata.m_fontColour.Blue() / 255.0
                                        alpha:1.0]
        ];
    else
        [[NSColorPanel sharedColorPanel] setColor:[NSColor blackColor]];
#endif
    
    [NSApp runModalForWindow:fontPanel];
    
    // if we don't reenable it, FPShowHideFontPanel does not work
    [[fontPanel standardWindowButton:NSWindowCloseButton] setEnabled:YES] ;
#if wxOSX_USE_CARBON
    if( FPIsFontPanelVisible())
        FPShowHideFontPanel() ;
#else
    [fontPanel close];
#endif

    if ( [accessoryView closedWithOk])
    {
#if wxOSX_USE_COCOA
        NSFont* theFont = [fontPanel panelConvertFont:[NSFont userFontOfSize:0]];

        fontdata.m_chosenFont = wxFont( theFont );

        //Get the shared color panel along with the chosen color and set the chosen color
        NSColor* theColor = [[[NSColorPanel sharedColorPanel] color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];

        fontdata.m_fontColour.Set((unsigned char) ([theColor redComponent] * 255.0),
                                    (unsigned char) ([theColor greenComponent] * 255.0),
                                    (unsigned char) ([theColor blueComponent] * 255.0));
#endif
        retval = wxID_OK ;
    }
    [fontPanel setAccessoryView:nil];
    [accessoryView release];

    return retval ;
}

#else

#if USE_NATIVE_FONT_DIALOG_FOR_MACOSX

IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)

// Cocoa headers

#import <AppKit/NSFont.h>
#import <AppKit/NSFontManager.h>
#import <AppKit/NSFontPanel.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSColorPanel.h>

// ---------------------------------------------------------------------------
// wxWCDelegate - Window Closed delegate
// ---------------------------------------------------------------------------

@interface wxWCDelegate : NSObject
{
    bool m_bIsClosed;
}

// Delegate methods
- (id)init;
- (BOOL)windowShouldClose:(id)sender;
- (BOOL)isClosed;
@end // interface wxNSFontPanelDelegate : NSObject

@implementation wxWCDelegate : NSObject

- (id)init
{
    [super init];
    m_bIsClosed = false;

    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    m_bIsClosed = true;

    [NSApp abortModal];
    [NSApp stopModal];
    return YES;
}

- (BOOL)isClosed
{
    return m_bIsClosed;
}

@end // wxNSFontPanelDelegate

// ---------------------------------------------------------------------------
// wxWCODelegate - Window Closed or Open delegate
// ---------------------------------------------------------------------------

@interface wxWCODelegate : NSObject
{
    bool m_bIsClosed;
    bool m_bIsOpen;
}

// Delegate methods
- (id)init;
- (BOOL)windowShouldClose:(id)sender;
- (void)windowDidUpdate:(NSNotification *)aNotification;
- (BOOL)isClosed;
- (BOOL)isOpen;
@end // interface wxNSFontPanelDelegate : NSObject

@implementation wxWCODelegate : NSObject

- (id)init
{
    [super init];
    m_bIsClosed = false;
    m_bIsOpen = false;

    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    m_bIsClosed = true;
    m_bIsOpen = false;

    [NSApp abortModal];
    [NSApp stopModal];
    return YES;
}

- (void)windowDidUpdate:(NSNotification *)aNotification
{
    if (m_bIsOpen == NO)
    {
        m_bIsClosed = false;
        m_bIsOpen = true;

        [NSApp abortModal];
        [NSApp stopModal];
    }
}

- (BOOL)isClosed
{
    return m_bIsClosed;
}

- (BOOL)isOpen
{
    return m_bIsOpen;
}

@end // wxNSFontPanelDelegate

// ---------------------------------------------------------------------------
// wxFontDialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_fontData = data;

    //autorelease pool - req'd for carbon
    NSAutoreleasePool *thePool;
    thePool = [[NSAutoreleasePool alloc] init];

    //Get the initial wx font
    wxFont& thewxfont = m_fontData.m_initialFont;

    //if the font is valid set the default (selected) font of the
    //NSFontDialog to that font
    if (thewxfont.Ok())
    {
        NSFontTraitMask theMask = 0;

        if(thewxfont.GetStyle() == wxFONTSTYLE_ITALIC)
            theMask |= NSItalicFontMask;

        if(thewxfont.IsFixedWidth())
            theMask |= NSFixedPitchFontMask;

        NSFont* theDefaultFont =
            [[NSFontManager sharedFontManager] fontWithFamily:
                                                    wxNSStringWithWxString(thewxfont.GetFaceName())
                                            traits:theMask
                                            weight:thewxfont.GetWeight() == wxBOLD ? 9 :
                                                    thewxfont.GetWeight() == wxLIGHT ? 0 : 5
                                            size: (float)(thewxfont.GetPointSize())
            ];

        wxASSERT_MSG(theDefaultFont, wxT("Invalid default font for wxCocoaFontDialog!"));

        //Apple docs say to call NSFontManager::setSelectedFont
        //However, 10.3 doesn't seem to create the font panel
        //is this is done, so create it ourselves
        [[NSFontPanel sharedFontPanel] setPanelFont:theDefaultFont isMultiple:NO];

    }

    if(m_fontData.m_fontColour.Ok())
        [[NSColorPanel sharedColorPanel] setColor:
            [NSColor colorWithCalibratedRed:m_fontData.m_fontColour.Red() / 255.0
                                        green:m_fontData.m_fontColour.Green() / 255.0
                                        blue:m_fontData.m_fontColour.Blue() / 255.0
                                        alpha:1.0]
        ];
    else
        [[NSColorPanel sharedColorPanel] setColor:[NSColor blackColor]];

    //We're done - free up the pool
    [thePool release];

    return true;
}

int wxFontDialog::ShowModal()
{
    //Start the pool.  Required for carbon interaction
    //(For those curious, the only thing that happens
    //if you don't do this is a bunch of error
    //messages about leaks on the console,
    //with no windows shown or anything).
    NSAutoreleasePool *thePool;
    thePool = [[NSAutoreleasePool alloc] init];

    //Get the shared color and font panel
    NSFontPanel* theFontPanel = [NSFontPanel sharedFontPanel];
    NSColorPanel* theColorPanel = [NSColorPanel sharedColorPanel];

    //Create and assign the delegates (cocoa event handlers) so
    //we can tell if a window has closed/open or not
    wxWCDelegate* theFPDelegate = [[wxWCDelegate alloc] init];
    [theFontPanel setDelegate:theFPDelegate];

    wxWCODelegate* theCPDelegate = [[wxWCODelegate alloc] init];
    [theColorPanel setDelegate:theCPDelegate];

    //
    //  Begin the modal loop for the font and color panels
    //
    //  The idea is that we first make the font panel modal,
    //  but if the color panel is opened, unless we stop the
    //  modal loop the color panel opens behind the font panel
    //  with no input acceptable to it - which makes it useless.
    //
    //  So we set up delegates for both the color and font panels,
    //  and the if the font panel opens the color panel, we
    //  stop the modal loop, and start a separate modal loop for
    //  the color panel until the color panel closes, switching
    //  back to the font panel modal loop once it does close.
    //
    wxDialog::OSXBeginModalDialog();
    do
    {
        //
        //  Start the font panel modal loop
        //
        NSModalSession session = [NSApp beginModalSessionForWindow:theFontPanel];
        for (;;)
        {
            [NSApp runModalSession:session];

            //If the font panel is closed or the font panel
            //opened the color panel, break
            if ([theFPDelegate isClosed] || [theCPDelegate isOpen])
                break;
        }
        [NSApp endModalSession:session];

        //is the color panel open?
        if ([theCPDelegate isOpen])
        {
            //
            //  Start the color panel modal loop
            //
            NSModalSession session = [NSApp beginModalSessionForWindow:theColorPanel];
            for (;;)
            {
                [NSApp runModalSession:session];

                //If the color panel is closed, return the font panel modal loop
                if ([theCPDelegate isClosed])
                    break;
            }
            [NSApp endModalSession:session];
        }
        //If the font panel is still alive (I.E. we broke
        //out of its modal loop because the color panel was
        //opened) return the font panel modal loop
    }while([theFPDelegate isClosed] == NO);
    wxDialog::OSXEndModalDialog();
    
    //free up the memory for the delegates - we don't need them anymore
    [theFPDelegate release];
    [theCPDelegate release];

    //Get the font the user selected
    NSFont* theFont = [theFontPanel panelConvertFont:[NSFont userFontOfSize:0]];

    //Get more information about the user's chosen font
    NSFontTraitMask theTraits = [[NSFontManager sharedFontManager] traitsOfFont:theFont];
    int theFontWeight = [[NSFontManager sharedFontManager] weightOfFont:theFont];
    int theFontSize = (int) [theFont pointSize];

    //Set the wx font to the appropriate data
    if(theTraits & NSFixedPitchFontMask)
        m_fontData.m_chosenFont.SetFamily(wxTELETYPE);

    m_fontData.m_chosenFont.SetFaceName(wxStringWithNSString([theFont familyName]));
    m_fontData.m_chosenFont.SetPointSize(theFontSize);
    m_fontData.m_chosenFont.SetStyle(theTraits & NSItalicFontMask ? wxFONTSTYLE_ITALIC : 0);
    m_fontData.m_chosenFont.SetWeight(theFontWeight < 5 ? wxLIGHT :
                                    theFontWeight >= 9 ? wxBOLD : wxNORMAL);

    //Get the shared color panel along with the chosen color and set the chosen color
    NSColor* theColor = [[theColorPanel color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];

    m_fontData.m_fontColour.Set((unsigned char) ([theColor redComponent] * 255.0),
                                (unsigned char) ([theColor greenComponent] * 255.0),
                                (unsigned char) ([theColor blueComponent] * 255.0));

    //Friendly debug stuff
#ifdef FONTDLGDEBUG
    wxPrintf(wxT("---Font Panel---\n--NS--\nSize:%f\nWeight:%i\nTraits:%i\n--WX--\nFaceName:%s\nPointSize:%i\nStyle:%i\nWeight:%i\nColor:%i,%i,%i\n---END Font Panel---\n"),

                (float) theFontSize,
                theFontWeight,
                theTraits,

                m_fontData.m_chosenFont.GetFaceName().c_str(),
                m_fontData.m_chosenFont.GetPointSize(),
                m_fontData.m_chosenFont.GetStyle(),
                m_fontData.m_chosenFont.GetWeight(),
                    m_fontData.m_fontColour.Red(),
                    m_fontData.m_fontColour.Green(),
                    m_fontData.m_fontColour.Blue() );
#endif

    //Release the pool, we're done :)
    [thePool release];

    //Return ID_OK - there are no "apply" buttons or the like
    //on either the font or color panel
    return wxID_OK;
}

//old api stuff (REMOVE ME)
bool wxFontDialog::IsShown() const
{
    return false;
}

#endif // 10.2+

#endif
