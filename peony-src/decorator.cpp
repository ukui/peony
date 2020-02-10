#include "decorator.h"

#include <QWidget>
#include <QX11Info>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "mwmutil.h"

/*!
 * \brief Decorator::wrapWindow
 * \param w
 * \details
 * Make window do not draw title bar but managed by window manager.
 */
void Decorator::wrapWindow(QWidget *w)
{
    MotifWmHints new_hints;

    /* initialize to zero to avoid writing uninitialized data to socket */
    memset(&new_hints, 0, sizeof(new_hints));
    new_hints.flags = MWM_HINTS_DECORATIONS;
    new_hints.decorations = 0;

    Atom frame_extents;
    ulong data[4] = {
        10,
        10,
        10,
        10
    };

    frame_extents = XInternAtom (QX11Info::display(), "_GTK_FRAME_EXTENTS", true);
    if (frame_extents == None)
        qDebug()<<"no frame extents";

    Atom mwm_hints;
    uchar *hints_data;
    MotifWmHints *hints;
    Atom type;
    int format;
    ulong nitems;
    ulong bytes_after;

    mwm_hints = XInternAtom(QX11Info::display(), "_MOTIF_WM_HINTS", true);
    if (mwm_hints == None)
        qDebug()<<"no mwm_hints";
    else {
        qDebug()<<w->winId();
        XChangeProperty(QX11Info::display(),
                        w->winId(),
                        frame_extents,
                        XA_CARDINAL,
                        32,
                        PropModeReplace,
                        (uchar*)&data,
                        4);
    }

    XGetWindowProperty (QX11Info::display(),
                        w->winId(),
                        mwm_hints,
                        0,
                        sizeof (MotifWmHints)/sizeof (long),
                        false,
                        AnyPropertyType,
                        &type,
                        &format,
                        &nitems,
                        &bytes_after,
                        &hints_data);

    hints = &new_hints;

    if (type == None)
        hints = &new_hints;
    else {
        hints = (MotifWmHints *)hints_data;

        if (new_hints.flags & MWM_HINTS_FUNCTIONS)
        {
            hints->flags |= MWM_HINTS_FUNCTIONS;
            hints->functions = new_hints.functions;
        }
        if (new_hints.flags & MWM_HINTS_DECORATIONS)
        {
            hints->flags |= MWM_HINTS_DECORATIONS;
            //effect in ukwm, but uneffect in kwin
            hints->decorations = MWM_DECOR_BORDER;
        }
    }

    //_MOTIF_WM_HINTS
    qDebug()<<XChangeProperty(QX11Info::display(),
                    w->winId(),
                    mwm_hints,
                    mwm_hints,
                    32,
                    PropModeReplace,
                    (uchar*)hints,
                    sizeof (MotifWmHints)/sizeof (long));
}

Decorator::Decorator(QObject *parent) : QObject(parent)
{

}
