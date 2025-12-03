//
//  GateController.hpp
//  Parking
//
//  Created by Михаил Конюхов on 03.12.2025.
//

#ifndef GateController_hpp
#define GateController_hpp

#include <stdio.h>
#include "SerialPort.hpp"
#include <unistd.h>

using namespace std;

class GateController {
private:
    SerialPort port;
    uint8_t deviceId;
public:
    bool init(const string& devicePath, const uint8_t& id);
    void openGate();
    void closeGate();
};
#endif /* GateController_hpp */
