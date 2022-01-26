#pragma once

// VNC
#include <rfb/rfb.h>
// X11/xlib
#include <X11/Xlib.h>

#include <string>

class logger {
public:
    static void info(const std::string& s);
    static void error(const std::string& s);
private:
    logger() = default;
};

class Configure {
private:
    int _width;
    int _height;
    int _depth;
public:
    Configure();
    Configure* setWidth(int w);
    Configure* setHeight(int h);
    Configure* setDepth(int d);

    int getWidth() const;
    int getHeight() const;
    int getDepth() const;
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