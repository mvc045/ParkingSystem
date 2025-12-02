//
//  ConfigLoader.cpp
//  Parking
//
//  Created by Михаил Конюхов on 02.12.2025.
//

#include "ConfigLoader.hpp"
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

bool ConfigLoader::load(const string& filename) {
    ifstream file(filename);
    
    if(!file.is_open()) {
        cerr << "Ошибка, не получилось открыть файл " << filename << "\n";
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        // игнорим коментарии и пустые строки
        if (line.empty() || line[0] == '#') { continue; }
        
        size_t separator = line.find('=');
        if (separator != string::npos) {
            string key = line.substr(0, separator);
            string value = line.substr(separator + 1);
            
            settings[key] = value;
        }
    }
    
    return true;
};

string ConfigLoader::getString(const string& key, const string& defaultValue) {
    if(settings.count(key)) {
        try {
            return settings[key];
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
};

int ConfigLoader::getInt(const string& key, const int defaultValue) {
    if(settings.count(key)) {
        try {
            return stoi(settings[key]);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}
