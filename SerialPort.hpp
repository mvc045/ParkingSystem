//
//  SerialPort.hpp
//  Parking
//
//  Created by Михаил Конюхов on 01.12.2025.
//

#ifndef SerialPort_hpp
#define SerialPort_hpp

#include <stdio.h>
#include <string>
#include <vector>

// Драйвер для шлагбаума
class SerialPort {
private:
    int fileDescriptor;
    bool isConnect;

public:
    SerialPort();
    ~SerialPort();
    
    bool connect(const std::string& portName);
    void disconnect();
    bool sendData(const std::string& data);
    bool sendBytes(const std::vector<uint8_t>& data);
    int readBytes(std::vector<uint8_t>& buffer, int expectedLength, int timeout);
};

#endif /* SerialPort_hpp */
