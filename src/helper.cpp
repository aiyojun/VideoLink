#include "helper.h"
#include <cstdio>

// --- logger ---
void logger::info(const std::string& s)
{printf("[info ] %s\n", s.c_str());}
void logger::error(const std::string &s)
{printf("\033[31;1m[error] %s\033[0m\n", s.c_str());}

// --- Configure ---
Configure::Configure(): _x0(0), _y0(0), _width(0), _height(0), _depth(0) {}
Configure* Configure::setX0(int x0) {_x0 = x0;return this;}
Configure* Configure::setY0(int y0) {_y0 = y0;return this;}
Configure* Configure::setWidth(int w) {_width = w;return this;}
Configure* Configure::setHeight(int h) {_height = h;return this;}
Configure* Configure::setDepth(int d) {_depth = d;return this;}
int Configure::getWidth() const {return _width;}
int Configure::getHeight() const {return _height;}
int Configure::getDepth() const {return _depth;}
int Configure::getX0() const {return _x0;}
int Configure::getY0() const {return _y0;}

// --- VncHelper ---
VncHelper::VncHelper(Configure* cfg): _cfg(cfg), _server{} {}

Configure         *gCfg = nullptr;
VncHelper         *gVnc = nullptr;

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
    _server->ptrAddEvent = &VncHelper::defaultPtrHandler;
    gCfg   = _cfg;
    gVnc   = this;
    rfbInitServer(_server);
    return this;
}

VncHelper *VncHelper::setTheX(XlibHelper *x) {_theX = x;return this;}

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
    rfbRunEventLoop(_server, -1, FALSE);
    return this;
}

VncHelper* VncHelper::flush() {
    rfbMarkRectAsModified(_server, 0, 0, _cfg->getWidth(), _cfg->getHeight());
    rfbProcessEvents(_server, 100000);
    return this;
}

bool VncHelper::getState()
{return rfbIsActive(_server);}

XlibHelper *VncHelper::getTheX() {return _theX;}

int lastHit = 0;

void VncHelper::defaultPtrHandler(int buttonMask, int x, int y, rfbClientPtr cl)
{
    if (!buttonMask) {
        lastHit = 0;
        return;
    }
    x = gCfg->getX0() + x; y = gCfg->getY0() + y;
    if (lastHit != buttonMask) {
        gVnc->getTheX()->cursor(buttonMask, x, y);
        lastHit = buttonMask;
    }

    // ?The following method is strange! Maybe it's empty implement.?
    // rfbDefaultPtrAddEvent(buttonMask, x, y, cl);
}

// --- XlibHelper ---
XlibHelper::XlibHelper(Configure* cfg): _cfg(cfg), _pImg(nullptr) {}

XlibHelper* XlibHelper::init() {
    _display = XOpenDisplay(nullptr);
    _window  = DefaultRootWindow(_display);
    return this;
}

char* XlibHelper::grab() {
    _pImg = XGetImage(_display, _window,
                      _cfg->getX0(), _cfg->getY0(),
                      _cfg->getWidth(), _cfg->getHeight(),
                      XAllPlanes(), ZPixmap);
    return _pImg->data;
}

XlibHelper* XlibHelper::close() {
    // XFree(childWindows);
    XCloseDisplay(_display);
    return this;
}

void XlibHelper::cursor(int hit, int x, int y) {
    XWarpPointer(_display, None, _window, 0, 0, 0, 0, x, y);
    int button;
    logger::info("cursor action: " + std::to_string(hit));
    switch (hit) {
        case 1: button = Button1; break; // I'm not sure whether 'Button1' is left click.
        case 2: button = Button2; break; // ??? Maybe we should refer to xlib document.
        case 4: button = Button3; break; //
        default: return;
    }
    XEvent event;
    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    XQueryPointer(_display, RootWindow(_display, DefaultScreen(_display)),
                  &event.xbutton.root, &event.xbutton.window,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y,
                  &event.xbutton.state);
    event.xbutton.subwindow = event.xbutton.window;
    while (event.xbutton.subwindow) {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(_display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }
    if(XSendEvent(_display, PointerWindow, True, 0xfff, &event) == 0)
        logger::error("XLib process mouse click event error");
    XFlush(_display);
    usleep(50000);
    event.type = ButtonRelease;
    event.xbutton.state = 0x100;
    if(XSendEvent(_display, PointerWindow, True, 0xfff, &event) == 0)
        logger::error("XLib process mouse release event error");
    XFlush(_display);
}
