import logging
from pymodbus.server import StartSerialServer
from pymodbus.datastore import ModbusSequentialDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.transaction import ModbusRtuFramer

logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.DEBUG)

 # Эмулятор шлагбаума через Modbus RTU
 # Нужные библиотеки: pip install pymodbus==3.6.1 pyserial

def run_fake_barrier():
    store = ModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*100),
        co=ModbusSequentialDataBlock(0, [0]*100),
        hr=ModbusSequentialDataBlock(0, [0]*100),
        ir=ModbusSequentialDataBlock(0, [0]*100)
    )

    # context, это память нашего  шалгбаума
    context = ModbusServerContext(slaves=store, single=True)

    StartSerialServer(
        context=context,  
        identity=None,
        port='/dev/ttys340',
        framer=ModbusRtuFramer,
        baudrate=9600,
        parity='N',
        stopbits=1,
        bytesize=8
    )

if __name__ == "__main__":
    run_fake_barrier()