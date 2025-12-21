//
//  Middleware.hpp
//  Parking
//
//  Created by Михаил Конюхов on 21.12.2025.
//

#ifndef Middleware_hpp
#define Middleware_hpp

#include <stdio.h>
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <functional>
#include <iostream>

using namespace std;
using json = nlohmann::json;
using Handler = std::function<void(const httplib::Request&, httplib::Response&)>;

class Middleware {
public:
    static Handler withAuth(const string& apiKey, Handler targetHandler) {
        return [apiKey, targetHandler](const httplib::Request& req, httplib::Response& res) {
            // Проверяем auth хедер
            if (!req.has_header("Authorization")) {
                json j;
                j["error"] = "Неверный api key";
                res.status = 401;
                res.set_content(j.dump(), "application/json");
                return;
            }
            
            string authToken = req.get_header_value("Authorization");
            
            // Ожидаем формат ключа с Bearer <token>
            string prefix = "Bearer ";
            if (authToken.substr(0, prefix.size()) != prefix) {
                json j;
                j["error"] = "Неверный api key";
                res.status = 401;
                res.set_content(j.dump(), "application/json");
                return;
            }
            
            string token = authToken.substr(prefix.size());
            
            // Сравниваем токен, с тем что в конфиге
            if (token != apiKey) {
                json j;
                j["error"] = "Неверный api key";
                res.status = 403;
                res.set_content(j.dump(), "application/json");
                return;
            }
            
            // Если все ок
            targetHandler(req, res);
        };
    }
};

#endif /* Middleware_hpp */
