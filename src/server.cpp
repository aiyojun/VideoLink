#include <iostream>
#include "helper.h"

int main(int argc,char** argv) {
    /**
     * We should put all settings
     * into the container.
     */
    auto* pCfg = new Configure();
    pCfg->setX0(600)
        ->setY0(0)
        ->setWidth(400)
        ->setHeight(800);
    /**
     * Then, do some initializations.
     */
    VncHelper vnc(pCfg);
    XlibHelper xserver(pCfg);
    vnc.init(&argc, argv);
    vnc.setTheX(&xserver);
    /**
     * Finally, grab a image from the
     * operating system and launch the
     * working thread.
     */
    logger::info("VideoLinkServer start ...");
    xserver.init();
    while (vnc.getState()) {
        vnc.update(xserver.grab())->flush();
    }
//    vnc.update(xserver.init()->grab())->start();
    /**
     * Although, here won't execute,
     * I just want to tell you!
     */
    xserver.close();
    return 0;
}
