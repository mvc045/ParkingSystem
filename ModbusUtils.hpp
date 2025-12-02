//
//  ModbusUtils.hpp
//  Parking
//
//  Created by Михаил Конюхов on 01.12.2025.
//

#ifndef ModbusUtils_hpp
#define ModbusUtils_hpp

#include <vector>
#include <cstdint>
#include <stdio.h>
using namespace std;

class ModbusUtils {
public:
    // Статический метод расчета CRC
    static uint16_t calculateCRC(const std::vector<uint8_t>& data) {
        uint16_t crc = 0xFFFF;
        
        for (size_t pos = 0; pos < data.size(); pos++) {
            crc ^= (uint16_t)data[pos]; // XOR byte into least sig. byte of crc
            
            for (int i = 8; i != 0; i--) { // Loop over each bit
                if ((crc & 0x0001) != 0) { // If the LSB is set
                    crc >>= 1; // Shift right and XOR 0xA001
                    crc ^= 0xA001;
                } else { // Else LSB is not set
                    crc >>= 1; // Just shift right
                }
            }
        }
        // ВАЖНО: Modbus ожидает Little Endian (младший байт вперед) в конце,
        // но сам расчет выдает число. Разделять на байты будем при отправке.
        return crc;
    }
};

struct ModbusFrame {
    uint8_t address; // Адрес устройства
    uint8_t commandCode; // Команда
    uint16_t registerAddr; // Куда пишем
    uint16_t value; // Что пишем
    
    vector<uint8_t> serialize() {
        vector<uint8_t> buffer;
        
        buffer.push_back(address);
        buffer.push_back(commandCode);
        
        // Modbus Big-Endian (Старший байт первый)
        buffer.push_back((registerAddr >> 8) & 0xFF);
        buffer.push_back(registerAddr & 0xFF);
        
        buffer.push_back((value >> 8) & 0xFF);
        buffer.push_back(value & 0xFF);
        
        // Считаем CRC
        uint16_t crc = ModbusUtils::calculateCRC(buffer);
        
        // Modbus Little-Endian (Младший байт первый)
        buffer.push_back(crc & 0xFF);
        buffer.push_back((crc >> 8) & 0xFF);
        
        return buffer;
    }
    
};

#endif /* ModbusUtils_hpp */
