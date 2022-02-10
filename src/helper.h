#pragma once

#include <rfb/rfb.h>
#include <X11/Xlib.h>

#include <string>

class logger {
public:
    static void info (const std::string& s);
    static void error(const std::string& s);
private:
    logger() = default;
};

class Configure {
private:
    int _x0;
    int _y0;
    int _width;
    int _height;
    int _depth;
public:
    Configure();
    Configure* setX0(int x);
    Configure* setY0(int y);
    Configure* setWidth (int w);
    Configure* setHeight(int h);
    Configure* setDepth (int d);
    int getWidth () const;
    int getHeight() const;
    int getX0 () const;
    int getY0 () const;
    int getDepth () const;
};

class XlibHelper;

class VncHelper {
public:
    VncHelper(Configure* cfg);
    VncHelper* init(int* argc,char** argv);
    VncHelper* setTheX(XlibHelper* x);
    VncHelper* update(char* data);
    VncHelper* start();
    VncHelper* flush();
    bool getState();
    XlibHelper* getTheX();
    static void defaultPtrHandler(int buttonMask, int x, int y, rfbClientPtr cl);
private:
    Configure       *_cfg;
    rfbScreenInfoPtr _server;
    XlibHelper      *_theX;
};

class XlibHelper {
public:
    XlibHelper(Configure* cfg);
    XlibHelper* init();
    char*       grab();
    XlibHelper* close();
    void cursor(int hit, int x, int y);
private:
    Configure *_cfg;
    XImage    *_pImg;
    Display   *_display;
    Window     _window;
};