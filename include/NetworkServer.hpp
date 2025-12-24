//
//  NetworkServer.hpp
//  Parking
//
//  Created by Михаил Конюхов on 24.12.2025.
//

#ifndef NetworkServer_hpp
#define NetworkServer_hpp

#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include "App.h"
#include "json.hpp"
#include "GateController.hpp"
#include "Database.hpp"

using json = nlohmann::json;

class NetworkServer {
private:
    struct PerSocketData {};
    using JSONHandler = function<json(json requestBody)>;
    
    GateController& controller;
    Database& db;
    string apiKey;
    
    uWS::Loop* loop = nullptr;
    uWS::App* globalApp = nullptr;
    
    thread serverThread;
    
    // В uWebSocket нету нормального парсера Body из post запроса, сделал этот helper.
    void postJSON(uWS::HttpResponse<false>* res, JSONHandler handler);
public:
    NetworkServer(GateController& gc, Database& db, const string& key);
    void start(int port);
    void broadcastStatus(const string& status);
};

#endif /* NetworkServer_hpp */
