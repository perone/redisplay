import serial
import json

ser = serial.Serial("/dev/ttyUSB0", 57600)
print "Starting..."

d = {
    "CMD": "update_basic",
    "clients" : "102",
    "memory" : "124M",
    "ops/s": "23",
}

data = json.dumps(d) + "\n"
ser.write(data)
print "Wrote:", data
ser.close()

#while True:
#    print ser.readline()
#    print "."