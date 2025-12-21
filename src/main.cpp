//
//  main.cpp
//  Parking
//
//  Created by Михаил Конюхов on 01.12.2025.
//

#include "SerialPort.hpp"
#include "GateController.hpp"
#include "ModbusUtils.hpp"
#include "ConfigLoader.hpp"
#include "Database.hpp"
#include <iostream>
#include <unistd.h>
#include "Middleware.hpp"

// HTTP сервер
#include "httplib.h"
// Парсим json
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Путь до файла с конфигурацией
string pathConfig = "/Users/mvc/Documents/C++/SysCalls/Parking/config.txt";
ConfigLoader config;

// cmake -G Xcode -B build_xcode

int main(int argc, const char * argv[]) {
    // Загружаем конфиг
    if (!config.load(pathConfig)) {
        cout << "Файл не найден\n";
    }
    
    int deviceId = config.getInt("barrier_id");
    string portName = config.getString("serial_port");
    string apiKey = config.getString("api_key");
    
    SerialPort port;
    
    if(!port.connect(portName)) {
        cerr << "Ошибка инициализации контроллера управления шлагбаумом.\n";
        return 1;
    }
    
    port.flush();
    
    GateController gateController(port, static_cast<uint8_t>(deviceId));
    
    // Создаем файл с бд
    Database db("parking_01.db");
    
    // Поднимаем HTTP сервер
    httplib::Server svr;
    
    svr.Get("/history", [&](const httplib::Request& req, httplib::Response& res) {
        json history = db.getHistory();
        res.set_content(history.dump(4), "application/json");
    });
    
    svr.Get("/status", [&](const httplib::Request& req, httplib::Response& res) {
        bool isOpen = gateController.isGateOpen();
        
        json response;
        response["device_id"] = deviceId;
        response["status"] = isOpen ? "open" : "closed";
        response["timestamp"] = time(nullptr);
        res.set_content(response.dump(), "application/json");
    });
    
    svr.Post("/open", Middleware::withAuth(apiKey, [&](const httplib::Request& req, httplib::Response& res) {
        json response;
        
        try {
            gateController.openGate();
            response["ok"] = true;
            response["timestamp"] = time(nullptr);
            
            db.logEvent("INFO", "Шлагбаум открыт", deviceId);
            
            res.set_content(response.dump(), "application/json");
        } catch (const exception& e) {
            // Отлавливаем ошибки
            response["ok"] = false;
            response["message"] = e.what();
            
            db.logEvent("ERROR", string("Ошибка: ") + e.what(), deviceId);
            
            res.status = 500;
            res.set_content(response.dump(), "application/json");
        }
    }));
    
    svr.Post("/close", Middleware::withAuth(apiKey, [&](const httplib::Request& req, httplib::Response& res) {
        json response;
        
        try {
            gateController.closeGate();
            
            response["ok"] = true;
            response["timestamp"] = time(nullptr);
            
            db.logEvent("INFO", "Шлагбаум закрыт", deviceId);
            
            res.set_content(response.dump(), "application/json");
        } catch (const exception& e) {
            response["ok"] = false;
            response["message"] = e.what();
            
            db.logEvent("ERROR", string("Ошибка: ") + e.what(), deviceId);
            
            res.status = 500;
            res.set_content(response.dump(), "application/json");
        }
    }));
    
    int portHTTP = config.getInt("port_http");
    
    cout << "HTTP server run ... \n";
    svr.listen("0.0.0.0", portHTTP);
    
    return 0;
}
