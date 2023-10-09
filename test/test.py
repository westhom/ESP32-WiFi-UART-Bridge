import serial
import time

with serial.Serial('/dev/cu.usbserial-AH0183VP', 250000) as ser:
  print("opened serial port")
  while True:
    line = ser.readline()
    print('>> ' + line.decode('ascii').strip())
    out = 'M118 ' + line.decode('ascii').strip() + '\n'
    print('<< ' + out)
    ser.write(bytes(out, 'ascii'))

    time.sleep(0.01)

