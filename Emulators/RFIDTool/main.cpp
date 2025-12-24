//
//  main.cpp
//  RfidTool
//
//  Created by Михаил Конюхов on 21.12.2025.
//

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

using namespace std;

class SimpleSender {
    int fd;
public:
    SimpleSender() : fd(-1) {}
    ~SimpleSender() { if (fd != -1) close(fd); }
    
    bool connect(const string& portName) {
        fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) return false;
        fcntl(fd, F_SETFL, 0);
        return true;
    }
    
    void send(const string& text) {
        if (fd == -1) return;
        string data = text + "\n\r";
        write(fd, data.c_str(), data.size());
        cout << "Отправлено: " << text << "\n";
    }
    
};

void showMenu() {
    cout << "\n--- RFID Эмулятор ---\n";
    cout << "[1] Карта жильца\n";
    cout << "[2] Неизвестная карта\n";
    cout << "[3] Ввести RFID код\n";
    cout << "[3] Стресс-тест\n";
    cout << "[q] Выход\n";
}


int main(int argc, const char * argv[]) {
    string portName;
    
    if (argc > 1) {
        portName = argv[1];
    } else {
        portName = "/dev/ttys027";
//        cout << "Введите порт в который будем писать: \n";
//        cin >> portName;
    }
    
    SimpleSender sender;
    if (!sender.connect(portName)) {
        cerr << "Ошбика: Не удалось открыть порт " << portName << "\n";
        return 1;
    }
    
    cout << "Подключено к " << portName << "\n";
    
    char choice;
    while (true) {
        showMenu();
        cin >> choice;
        
        if (choice == 'q') break;
        switch (choice) {
            case '1': sender.send("CARD_1112"); break;
            case '2': sender.send("CARD_1122"); break;
            default: cout << "Неизвестная команда\n";
        }
        
    }
    
    return 0;
}
