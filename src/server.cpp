#include <iostream>
#include "helper.h"

int main(int argc,char** argv) {
    /** We should put all settings into the container. */
    auto* pCfg = new Configure();
    pCfg->setWidth(1200)->setHeight(600);
    /** Then, do some initializations. */
    VncHelper vnc(pCfg);
    vnc.init(&argc, argv);
    XlibHelper xserver(pCfg);
    /** Finally, grab a image from the
     *  operating system and launch the
     *  working thread.
     */
    logger::info("VideoLinkServer start ...");
    vnc.update(xserver.init()->grab())->start();
    /** Although, here won't execute,
     *  I just want to tell you!
     */
    xserver.close();
    return 0;
}
