import serial
import json
import random
import time

ser = serial.Serial("/dev/ttyUSB0", 9600)
print "Starting..."

while True:
    d = {
        "CMD": "update_basic",
        "clients" : "%d" % random.randint(0, 1000),
        "memory" : "%dM" % random.randint(0, 1000),
        "ops/s": "%d" % random.randint(0, 1000),
    }

    data = json.dumps(d) + '\n'
    ser.write(data)
    print "Wrote:", data
    ser.flush()
    time.sleep(0.3)

ser.close()

#while True:
#    print ser.readline()
#    print "."