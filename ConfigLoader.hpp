//
//  ConfigLoader.hpp
//  Parking
//
//  Created by Михаил Конюхов on 02.12.2025.
//

#ifndef ConfigLoader_hpp
#define ConfigLoader_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

class ConfigLoader {
private:
    map<string, string> settings;
public:
    // Загружаем файл
    bool load(const string& filename);
    // Получаем строку
    string getString(const string& key, const string& defaultValue = "");
    // Получаем число
    int getInt(const string& key, int defaultValue = 0);
};
#endif /* ConfigLoader_hpp */
