#include "helper.h"

Configure* Configure::setWidth(int w) {_width = w;return this;}
Configure* Configure::setHeight(int h) {_height = h;return this;}
Configure* Configure::setDepth(int d) {_depth = d;return this;}
int Configure::getWidth() {return _width;}
int Configure::getHeight() {return _height;}
int Configure::getDepth() {return _depth;}

VncHelper::VncHelper(Configure* cfg): _cfg(cfg) {

}

VncHelper* VncHelper::init(int* argc,char** argv) {
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


