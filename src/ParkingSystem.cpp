//
//  ParkingSystem.cpp
//  Parking
//
//  Created by Михаил Конюхов on 23.12.2025.
//

#include "ParkingSystem.hpp"
#include <iostream>

using namespace std;

ParkingSystem::ParkingSystem(): db("parking_01.db"), controller(gatePort, 0), networkServer(controller, db, "secret_password_123") {
}

bool ParkingSystem::init(const string& configPath) {
    // 1. Конфиг
    if (!config.load(configPath)) {
        return false;
    }
    string gatePortName = config.getString("serial_port");
    string rfidPortName = config.getString("rfid_port");
    int barrierId = config.getInt("barrier_id");
    string apiKey = config.getString("api_key");
    
    // 2. Оборудования
    
    // Шлагбаум
    if (!gatePort.connect(gatePortName)) {
        cerr << "Ошибка: Подключения к шлагбауму - " << gatePortName;
        return false;
    }
    gatePort.flush();
    
    // RFID
    if (!rfidReader.connect(rfidPortName)) {
        cerr << "Ошибка: Подключения к RFID - " << rfidPortName;
    }
    
    // 3.
    setup();
    return true;
}

void ParkingSystem::setup() {
    controller.setLogger([this](string type, string msg) {
        // Пишем в БД
        db.logEvent(type, msg, this->config.getInt("barrier_id"));
        // в Терминал
        cout << "[" << type << "] " << msg << "\n";
        // в WebSocket
        
        if (msg.find("открыт") != string::npos) {
            this->networkServer.broadcastStatus("Open");
        } else if (msg.find("закрыт") != string::npos) {
            this->networkServer.broadcastStatus("Closed");
        }
        
    });
    
    rfidReader.setCallBack([this](string cardCode) {
        this->processRFIDCard(cardCode);
    });
}

void ParkingSystem::processRFIDCard(const string& cardCode) {
    cout << "[RFID] Сканируем: " << cardCode;
    
    if (db.checkAccessRFID(cardCode)) {
        cout << "[RFID] Доступ получен для " << cardCode;
        db.logEvent("RFID", "Доступ получен для " + cardCode, config.getInt("barrier_id"));
    } else {
        cout << "[RFID] Нет доступа для - " << cardCode;
        db.logEvent("RFID", "Нет доступа для - " + cardCode, config.getInt("barrier_id"));
    }
}

void ParkingSystem::run() {
    rfidReader.start();
    int httpPort = config.getInt("port_http");
    networkServer.start(httpPort);
    
    cin.get();
}
