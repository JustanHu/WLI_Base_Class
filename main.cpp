#include <iostream>
#include "brukerWLI/brukerWLI.h"

int main() {
    std::string WLIIp = "192.168.250.6";
    brukerWLI WLITest(WLIIp,8888);
    WLITest.tcpBrukerConnect();
    WLITest.initTurret();
    return 0;
}
