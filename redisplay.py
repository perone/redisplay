import serial
import json
import random
import time

ser = serial.Serial("/dev/ttyUSB0", 9600)
print "Starting..."

while True:
    d = {
        "cmd": "update_basic",
        "clients" : "%d" % random.randint(0, 1000),
        "memory" : "%dM" % random.randint(0, 1000),
        "ops/s": "%d" % random.randint(0, 1000),
    }

    d2 = {
        "cmd": "update_advanced",
        "rej_conn" : "%d" % random.randint(0, 1000),
        "key_hits" : "%dM" % random.randint(0, 1000),
        "key_miss": "%d" % random.randint(0, 1000),
    }

    data = json.dumps(d) + '\n'
    ser.write(data)
    print "Wrote:", data
    ser.flush()
    time.sleep(0.3)

    data = json.dumps(d2) + '\n'
    ser.write(data)
    print "Wrote:", data
    ser.flush()
    time.sleep(0.3)


ser.close()

#while True:
#    print ser.readline()
#    print "."