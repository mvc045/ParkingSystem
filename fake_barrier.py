import logging
import threading
import tkinter as tk
from tkinter import ttk

from pymodbus.server import StartSerialServer
from pymodbus.datastore import ModbusSequentialDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.transaction import ModbusRtuFramer

logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.INFO)

 # Эмулятор шлагбаума через Modbus RTU
 # Нужные библиотеки: pip install pymodbus==3.6.1 pyserial

serial_port = '/dev/ttys004'  # Укажите COM-порт

class BarrierEmulator:
    def __init__(self, root, context):
        self.root = root
        self.context = context

        self.slave_id = 0x00
        self.speed = 1  # скорость шлагбаума

        # Создаем UI
        self.root.title("Эмулятор шлагбаума (modbus slave)")
        self.root.geometry("400x350")

        frame_status = tk.LabelFrame(self.root, text="Статус шлагбаума")
        frame_status.pack(fill="x", padx=10, pady=10)

        self.lbl_status = tk.Label(frame_status, text="Закрыт")
        self.lbl_status.pack(pady=10)

        # Прогресс бар
        self.progress = ttk.Progressbar(frame_status, orient="horizontal", length=300, mode="determinate")
        self.progress.pack(pady=10)
        self.progress['maximum'] = 100
        self.progress['value'] = 0

        # Данные Modbus
        frame_data = tk.LabelFrame(self.root, text="Данные Modbus")
        frame_data.pack(fill="x", padx=10, pady=10)

        self.lbl_coil = tk.Label(frame_data, text="Coil: 0")
        self.lbl_coil.pack(anchor="w", pady=5)

        self.lbl_di1 = tk.Label(frame_data, text="DI[1] (Is Closed): 1")
        self.lbl_di1.pack(anchor="w", pady=5)

        self.lbl_di2 = tk.Label(frame_data, text="DI[2] (Is Open): 0")
        self.lbl_di2.pack(anchor="w", pady=5)


        # 3. Управление датчиками (Simulate Inputs)
        frame_controls = tk.LabelFrame(self.root, text="Управление датчиками")
        frame_controls.pack(fill="x", padx=10, pady=10)

        # Кнопка для симуляции срабатывания датчика открытия
        self.btn_sensor = tk.Checkbutton(frame_controls, text="Симулировать датчик открытия", command=self.toggle_sensor)
        self.btn_sensor.pack(pady=5)

        self.sensor_val = tk.BooleanVar()
        self.btn_sensor.config(variable=self.sensor_val)

        self.updateUI()
    
    
    def get_store(self):
        return self.context[self.slave_id]

    def toggle_sensor(self):
        store = self.get_store()
        val = self.sensor_val.get()

        store.setValues(1, 0, [1 if val else 0])
        log.info(f"Датчик открытия установлен в: {val}")

    # Вызываем каждые 200мс для обновления UI
    def updateUI(self):
        store = self.get_store()
        
        # Читаем Coil[0]
        coils = store.getValues(1, 0, count=1)
        isOpenCommand = coils[0] if coils else False

        if isOpenCommand:
            # Открываем шлагбаум
            if self.progress['value'] < 100:
                self.progress['value'] += self.speed
        else:
            # Закрываем шлагбаум
            if self.progress['value'] > 0:
                self.progress['value'] -= self.speed
        
        # Определяем состояние концовиков
        is_fully_closed = (self.progress['value'] == 0)
        is_fully_open = (self.progress['value'] == 100)
        # Обновляем дискретные входы
        store.setValues(2, 1, [1 if is_fully_closed else 0])  # DI[1] - Is Closed
        store.setValues(2, 2, [1 if is_fully_open else 0])    # DI[2] - Is Open

        if is_fully_closed:
            self.lbl_status.config(text="Закрыт", fg="red")
        elif is_fully_open:
            self.lbl_status.config(text="Открыт", fg="green")
        else:
            value = self.progress['value']
            self.lbl_status.config(text=f"В движении {value} %", fg="orange")
        
        self.lbl_coil.config(text=f"Coil[0]: { 1 if isOpenCommand else 0 }")
        self.lbl_di1.config(text=f"DI[1] (Is Closed): {1 if is_fully_closed else 0}")
        self.lbl_di2.config(text=f"DI[2] (Is Open): {1 if is_fully_open else 0}")
        
        self.root.after(50, self.updateUI)


def run_modbus_server(context):
    StartSerialServer(
        context=context,  
        identity=None,
        port=serial_port,
        framer=ModbusRtuFramer,
        baudrate=9600,
        parity='N',
        stopbits=1,
        bytesize=8
    )


if __name__ == "__main__":

    # структура памяти Modbus
    store = ModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*100),
        co=ModbusSequentialDataBlock(0, [0]*100),
        hr=ModbusSequentialDataBlock(0, [0]*100),
        ir=ModbusSequentialDataBlock(0, [0]*100)
    )

    # адрес нашего устройства
    # { ID : Память }
    slaves = {
        0x00: store
    }
    
    # slaves - список устройств
    # single, если установить True, то будет работать с любым ID (в slaves передаем store)
    context = ModbusServerContext(slaves=slaves, single=False)
    
    server_thread = threading.Thread(target=run_modbus_server, args=(context,), daemon=True)
    server_thread.start()
    
    root = tk.Tk()
    app = BarrierEmulator(root, context)
    root.mainloop()


