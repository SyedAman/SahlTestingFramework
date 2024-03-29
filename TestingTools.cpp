#include "TestingTools.h"

#ifdef _WIN32

#include <Windows.h>

void TestingTools::MouseClick(int x, int y) {
    SetCursorPos(x, y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

TestingTools::CursorProperties TestingTools::GetCursorPosition() {
    POINT cursorPosition;
    GetCursorPos(&cursorPosition);

    return CursorProperties{cursorPosition.x, cursorPosition.y};
}

#endif // _WIN32

#ifdef __linux__

#include <X11/extensions/XTest.h>

#include <cstring>
#include <cstdio>
#include <chrono>
#include <thread>

TestingTools::CursorProperties TestingTools::GetCursorPosition() {
    Display *x11Display = XOpenDisplay(nullptr);
    Window rootWindow = DefaultRootWindow(x11Display);

    Window rootWindowUnderMouse;
    Window childWindowUnderMouse;
    int newMouseXUnderChildWindow;
    int newMouseYUnderChildWindow;

    unsigned int bitMaskForModifierKeys;

    CursorProperties cursorProperties{};

    XQueryPointer(
            x11Display,
            rootWindow,
            &rootWindowUnderMouse,
            &childWindowUnderMouse,
            &cursorProperties.x,
            &cursorProperties.y,
            &newMouseXUnderChildWindow,
            &newMouseYUnderChildWindow,
            &bitMaskForModifierKeys
    );

    XCloseDisplay(x11Display);

    return cursorProperties;
}

Window TestingTools::FindWindowByName(Display *display, Window root, const char *name) {
    Window parent;
    Window *children;
    unsigned int numberOfChildren;
    char *windowName;

    bool baseCase = XFetchName(display, root, &windowName) && strcmp(name, windowName) == 0;
    if (baseCase) return root;

    if (XQueryTree(display, root, &root, &parent, &children, &numberOfChildren)) {
        for (unsigned int i = 0; i < numberOfChildren; i++) {
            Window result = FindWindowByName(display, children[i], name);
            if (result != None) return result;
        }
        XFree(children);
    }

    return 0;
}

XRRCrtcInfo TestingTools::GetPrimaryMonitor(Display *display) {
    Window root = DefaultRootWindow(display);

    XRRScreenResources *screenResources = XRRGetScreenResources(display, root);
    RROutput primaryOutput = XRRGetOutputPrimary(display, root);
    XRROutputInfo *outputInfo = XRRGetOutputInfo(display, screenResources, primaryOutput);

    XRRCrtcInfo primaryMonitorCrtcInfo{};

    for (int i = 0; i < screenResources->ncrtc; ++i) {
        XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(display, screenResources, screenResources->crtcs[i]);

        for (int j = 0; j < crtcInfo->noutput; ++j) {
            if (crtcInfo->outputs[j] == primaryOutput) {
                primaryMonitorCrtcInfo = *crtcInfo;
                break;
            }
        }
        XRRFreeCrtcInfo(crtcInfo);
    }

    XRRFreeOutputInfo(outputInfo);
    XRRFreeScreenResources(screenResources);

    return primaryMonitorCrtcInfo;
}

void TestingTools::MoveWindowToTopRight(Display *display, Window window, XRRCrtcInfo targetMonitor) {
    int monitorXOrigin = targetMonitor.x;
    int monitorYOrigin = targetMonitor.y;
    unsigned int monitorWidth = targetMonitor.width;

    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, window, &windowAttributes);

    XMoveWindow(display, window, monitorXOrigin + monitorWidth - windowAttributes.width, monitorYOrigin);
    XFlush(display);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void TestingTools::MouseClick(int x, int y) {
    Display *x11Display = XOpenDisplay(nullptr);
    Window rootWindow = DefaultRootWindow(x11Display);
    XWarpPointer(x11Display, None, rootWindow, 0, 0, 0, 0, x, y);

    XTestFakeButtonEvent(x11Display, 1, True, 0);
    XTestFakeButtonEvent(x11Display, 1, False, 0);
    XFlush(x11Display);
}

#endif // __linux__