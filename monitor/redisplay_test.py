import serial
import json
import random
import time

ser = serial.Serial("/dev/ttyUSB0", 9600)
print "Starting..."

while True:
    d = {
        "cmd": "update_basic",
        "clients" : "%d" % random.randint(0, 100),
        "memory" : "%dM" % random.randint(512, 820),
        "ops/s": "%d" % random.randint(0, 1000),
    }

    d2 = {
        "cmd": "update_advanced",
        "rej_conn" : "%d" % random.randint(0, 5),
        "key_hits" : "%d" % random.randint(0, 10000),
        "key_miss": "%d" % random.randint(0, 100),
    }

    d3 = {
        "cmd" : "update_ops_sec",
        "graph_values" : [random.randint(1, 40) for x in xrange(40)],
        "max_value" : "1020",
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

    data = json.dumps(d3) + '\n'
    ser.write(data)
    print "Wrote:", data, len(data)
    ser.flush()
    time.sleep(0.3)

ser.close()
