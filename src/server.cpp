#include <iostream>
#include "helper.h"

int main(int argc,char** argv) {
    auto* pCfg = new Configure();
    pCfg->setWidth(1200)->setHeight(600);
    VncHelper vnc(pCfg);
    vnc.init(&argc, argv);
    XlibHelper xserver(pCfg);
    std::cout << "VideoLinkServer start ...\n";
    vnc.update(xserver.init()->grab())->start();
    xserver.close();
    return 0;
}
