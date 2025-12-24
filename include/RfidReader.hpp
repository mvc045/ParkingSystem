//
//  RfidReader.hpp
//  Parking
//
//  Created by Михаил Конюхов on 21.12.2025.
//

#ifndef RfidReader_hpp
#define RfidReader_hpp

#include <stdio.h>
#include "SerialPort.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <iostream>

using namespace std;

class RfidReader {
private:
    SerialPort port;
    atomic<bool> running;
    thread worker;
    
    // callback: когда прочитали карту
    function<void(string)> onCardRead;
public:
    RfidReader() : running(false) {}
    
    bool connect(const string& portName) {
        return port.connect(portName);
    }
    
    void setCallBack(function<void(string)> cb) {
        onCardRead = cb;
    }
    
    void start() {
        running = true;
        
        worker = thread([this]() {
            while (running) {
                vector<uint8_t> buffer;
                int n = port.readBytes(buffer, 10, 1);
                
                if (n > 0) {
                    string cardCode(buffer.begin(), buffer.end());
                    cardCode.erase(remove(cardCode.begin(), cardCode.end(), '\n'), cardCode.end());
                    cardCode.erase(remove(cardCode.begin(), cardCode.end(), '\r'), cardCode.end());
                    if (!cardCode.empty() && onCardRead) {
                        onCardRead(cardCode);
                    }
                }
            }
        });
        
        worker.detach();
    }
    
    void stop() {
        running = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
};

#endif /* RfidReader_hpp */
