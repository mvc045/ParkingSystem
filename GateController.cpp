//
//  GateController.cpp
//  Parking
//
//  Created by Михаил Конюхов on 03.12.2025.
//

#include "GateController.hpp"
#include "ModbusUtils.hpp"
#include <iostream>
#include <unistd.h>

using namespace std;

bool GateController::init(const string& devicePath, const uint8_t& id) {
    deviceId = id;
    return port.connect(devicePath);
}

void GateController::openGate() {
    cout << "[Controller] Отправили команду на открытие\n";
    
    ModbusFrame frame = { deviceId, Command::WRITE_SINGL_COIL, 0x0000, Action::OPEN };
    auto rawData = frame.serialize();

    port.sendBytes(rawData);
    cout << "Отправили пакет Modbus размером " << rawData.size() << " байт\n";

    // по стандарту modbus, устройство должно прислать ответ (ACK)
    vector<uint8_t> response;
    int bytesRead = port.readBytes(response, 8, 2); // Ждем 8 байт, 2 секунды

    // Проверяем что вернулось 8 байт
    if (bytesRead != 8) {
        cerr << "[Controller] Ошибка, с ответом от шлагбаума что то не так\n";
        cerr << "[Controller] " << bytesRead << "\n";
        return;
    }

    // Проверяем СRС ответа, нужно что бы они совпадали
    uint16_t receivedCRC = response[6] | (response[7] << 8);
    vector<uint8_t> dataOnly(response.begin(), response.end() - 2);
    uint16_t calcCRC = ModbusUtils::calculateCRC(dataOnly);

    if (receivedCRC == calcCRC) {
        cout << "[Controller] CRC совпадают\n";
        cout << "[Controller] Шлагбаум открыт\n";
    } else {
        cerr << "[Controller] CRC не совпали\n";
    }
}

void GateController::closeGate() {
    cout << "[Controller] Отправили команду на закрытие\n";

    ModbusFrame frame = { deviceId, Command::WRITE_SINGL_COIL, 0x0000, Action::CLOSE };
    auto rawData = frame.serialize();
    port.sendBytes(rawData);
}
