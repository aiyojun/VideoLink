#include "helper.h"
#include <cstdio>

// --- logger ---
void logger::info(const std::string& s)
{printf("[info ] %s\n", s.c_str());}
void logger::error(const std::string &s)
{printf("\033[31;1m[error] %s\033[0m\n", s.c_str());}

// --- Configure ---
Configure::Configure(): _width(0), _height(0), _depth(0) {}
Configure* Configure::setWidth(int w) {_width = w;return this;}
Configure* Configure::setHeight(int h) {_height = h;return this;}
Configure* Configure::setDepth(int d) {_depth = d;return this;}
int Configure::getWidth() const {return _width;}
int Configure::getHeight() const {return _height;}
int Configure::getDepth() const {return _depth;}

// --- VncHelper ---
VncHelper::VncHelper(Configure* cfg): _cfg(cfg), _server{} {}

VncHelper* VncHelper::init(int* argc,char** argv) {
    if (_cfg->getWidth() <= 0 || _cfg->getHeight() <= 0) {
        logger::error("Keep width > 0 && height > 0!");
        exit(1);
    }
    _server = rfbGetScreen(
            argc, argv,
            _cfg->getWidth(),
            _cfg->getHeight(),
            8, 3, 4);
    _server->frameBuffer = static_cast<char *>(
            malloc(_cfg->getWidth() * _cfg->getHeight() * 4));
    return this;
}

VncHelper* VncHelper::update(char* data) {
    /**
     * Here, watch out!
     * Make sure the color sequence correct!
     * VNC is rgb, but X11 is bgr!
     */
    for (int k = 0; k < _cfg->getWidth() * _cfg->getHeight(); k++) {
        *(_server->frameBuffer + k * 4 + 0) = data[k * 4 + 2];
        *(_server->frameBuffer + k * 4 + 1) = data[k * 4 + 1];
        *(_server->frameBuffer + k * 4 + 2) = data[k * 4 + 0];
        *(_server->frameBuffer + k * 4 + 3) = data[k * 4 + 3];
    }
    return this;
}

VncHelper* VncHelper::start() {
    rfbInitServer(_server);
    rfbRunEventLoop(_server, -1, FALSE);
    return this;
}
// --- XlibHelper ---
XlibHelper::XlibHelper(Configure* cfg): _cfg(cfg), _pImg(nullptr) {}

XlibHelper* XlibHelper::init() {
    _display = XOpenDisplay(nullptr);
    _window  = DefaultRootWindow(_display);
    return this;
}

char* XlibHelper::grab() {
    _pImg = XGetImage(_display, _window, 0, 0, _cfg->getWidth(), _cfg->getHeight(), XAllPlanes(), ZPixmap);
    return _pImg->data;
}

XlibHelper* XlibHelper::close() {
    // XFree(childWindows);
    XCloseDisplay(_display);
    return this;
}

