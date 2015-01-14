import serial
import json
import random
import time

from collections import deque

ser = serial.Serial("/dev/ttyUSB0", 9600)
print "Starting..."

ops_sec_deque = deque([random.randint(1, 30) for x in xrange(40)])

while True:
    _ = ops_sec_deque.popleft()
    ops_sec_deque.append(random.randint(1, 30))

    d = {
        "cmd": "update_basic",
        "clients" : "%d" % random.randint(80, 90),
        "memory" : "%dM" % random.randint(512, 820),
        "ops/s": "%d" % random.randint(0, 1000),
    }

    d2 = {
        "cmd": "update_advanced",
        "rej_conn" : "%d" % random.randint(0, 5),
        "key_hits" : "%d" % random.randint(1000, 10000),
        "key_miss": "%d" % random.randint(50, 300),
    }

    d3 = {
        "cmd" : "update_ops_sec",
        "graph_values" : list(ops_sec_deque),
        "max_value" : "1020",
    }


    data = json.dumps(d) + '\n'
    ser.write(data)
    print "Wrote:", data
    ser.flush()
    time.sleep(0.2)

    data = json.dumps(d2) + '\n'
    ser.write(data)
    print "Wrote:", data
    ser.flush()
    time.sleep(0.2)

    data = json.dumps(d3) + '\n'
    ser.write(data)
    print "Wrote:", data, len(data)
    ser.flush()
    time.sleep(0.2)

ser.close()
