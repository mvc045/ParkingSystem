//
//  ParkingSystem.hpp
//  Parking
//
//  Created by Михаил Конюхов on 23.12.2025.
//

#ifndef ParkingSystem_hpp
#define ParkingSystem_hpp

#include <stdio.h>
#include "ConfigLoader.hpp"
#include "Database.hpp"
#include "SerialPort.hpp"
#include "GateController.hpp"
#include "RfidReader.hpp"
#include "NetworkServer.hpp"

using namespace std;

class ParkingSystem {
private:
    ConfigLoader config;
    Database db;
    SerialPort gatePort;
    GateController controller;
    RfidReader rfidReader;
    NetworkServer networkServer;
    
    void setup();
    void processRFIDCard(const string& cardCode);
public:
    ParkingSystem();
    
    bool init(const string& configPath);
    void run();
};


#endif /* ParkingSystem_hpp */
