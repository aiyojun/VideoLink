#pragma once

// VNC
#include <rfb/rfb.h>
// X11/xlib
#include <X11/Xlib.h>

class Configure {
private:
    int _width;
    int _height;
    int _depth;
public:
    Configure* setWidth(int w);
    Configure* setHeight(int h);
    Configure* setDepth(int d);

    int getWidth();
    int getHeight();
    int getDepth();
};

class VncHelper {
public:
    VncHelper(Configure* cfg);
    VncHelper* init(int* argc,char** argv);
    VncHelper* update(char* data);
    VncHelper* start();
private:
    Configure* _cfg;
    rfbScreenInfoPtr _server;
};

class XlibHelper {
public:
    XlibHelper(Configure* cfg);
    XlibHelper* init();
    char* grab();
    XlibHelper* close();
private:
    Configure* _cfg;
    XImage* _pImg;
    Display* _display;
    Window _window;
};