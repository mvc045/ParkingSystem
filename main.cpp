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
#include <iostream>
#include <unistd.h>

using namespace std;

// Путь до файла с конфигурацией
string pathConfig = "/Users/mvc/Documents/C++/SysCalls/Parking/config.txt";
ConfigLoader config;

class Car {
private:
    string number;
    string owner;
    string phone;
public:
    Car(string numberAuto, string ownerAuto, string phoneOwner) {
        number = numberAuto;
        owner = ownerAuto;
        phone = phoneOwner;
    }
    string getNumber() { return number; }
    string getOwner() { return owner; }
};

class ParkingSpot {
private:
    string number;
    bool isOccupied;
    Car* car;
public:
    ParkingSpot(string numberParking) {
        number = numberParking;
        isOccupied = false;
        car = nullptr;
    }
    
    bool getIsOccupied() const { return isOccupied; }
    string getNumber() const { return number; }
    
    Car* getParkedCar() { return car; }
    
    void park(Car* c) {
        car = c;
        isOccupied = true;
        cout << "Место: " << number << " занято, машина: " << car->getNumber() << ". \n";
    }
    
    void leave() {
        if (isOccupied) {
            cout << "Место: " << number << " освободилось.\n";
            car = nullptr;
            isOccupied = false;
        } else {
            cout << "Место: " << number << "свободно.\n";
        }
    }
    
};

class ParkingManager {
private:
    vector<ParkingSpot> spots;
    GateController gateController;
public:
    ParkingManager(int totalSpots, const string& portPath, const int deviceId) {
        for (int i = 1; i <= totalSpots; i++) {
            spots.push_back(ParkingSpot(to_string(i)));
        }
        
        if (!gateController.init(portPath, deviceId)) {
            cerr << "Ошибка инициализации контроллера управления шлагбаумом.\n";
            abort();
        }
    }
    
    // Ищет свободные места, паркует занимает место для машины
    void parkCar(Car& car) {
        // Проверим нет ли такой машины уже на праковке
        for (ParkingSpot& spot: spots) {
            if (spot.getIsOccupied()) {
                Car* currentCar = spot.getParkedCar();
                if (currentCar->getNumber() == car.getNumber()) {
                    cout << "Машина с номерами " << car.getNumber() << " уже есть на парковке.\n";
                    return;
                }
            }
        }
        
        int sleepValue = config.getInt("timeout_open_gate");
        
        // Ищем свободные места, если находим паркуемся
        for (ParkingSpot& spot: spots) {
            if (!spot.getIsOccupied()) {
                // Занимаем место
                spot.park(&car);
                // Открываем шлагбаум
                gateController.openGate();
                sleep(sleepValue);
                gateController.closeGate();
                return;
            }
        }
        cout << "Извините, свободных мест нет.\n";
    }
    
    // Освободилось место
    void releaseSpot(string spotNumber) {
        auto it = find_if(spots.begin(), spots.end(), [&spotNumber](ParkingSpot& s) {
            return s.getNumber() == spotNumber;
        });
        
        // Если итератор не равен .end(), значит элемент найден
        if (it != spots.end()) {
            it->leave();
        } else {
            cout << "Парковочное место не найдено.\n";
        }
    }
    
};

int main(int argc, const char * argv[]) {
    // Загружаем конфиг
    if (!config.load(pathConfig)) {
        cout << "Файл не найден\n";
    }
    
    int deviceId = config.getInt("barrier_id");
    string portName = config.getString("serial_port");
    int parkingPlace = config.getInt("parking_place");
    
    // Парковка на 2 места
    ParkingManager manager(parkingPlace, portName, deviceId);
    
    Car car1("A001AA", "Иванов", "+7 996 558 91 96");
    Car car2("A002AA", "Петров", "+7 996 558 91 95");
    Car car3("E777KX", "Developer", "+7 996 558 91 94");
    Car car4("A004AA", "Иванов", "+7 996 558 91 93");
    
    manager.parkCar(car1);
//    manager.parkCar(car2);
//    manager.releaseSpot("1");
//    manager.parkCar(car3);
//    manager.parkCar(car4);
//    manager.parkCar(car1);
    
    return 0;
}
