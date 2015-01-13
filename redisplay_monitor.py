from __future__ import division

import time
import json
from collections import deque

import serial
import redis
import numpy as np

def build_update_basic(info):
    basic_info = {
        "cmd": "update_basic",
        "clients" : "%d" % info["connected_clients"],
        "memory" : info["used_memory_human"],
        "ops/s": "%d" % info["instantaneous_ops_per_sec"],
    }
    return basic_info

def build_update_advanced(info):
    advanced_info = {
        "cmd": "update_advanced",
        "rej_conn" : "%d" % info["rejected_connections"],
        "key_hits" : "%d" % info["keyspace_hits"],
        "key_miss": "%d" % info["keyspace_misses"],
    }
    return advanced_info

def build_update_ops_sec(ops_sec_deque):
    arr = np.array(ops_sec_deque)
    arr_max = arr.max()

    if arr_max > 0:
        arr *= 40/arr_max

    ops_sec = {
        "cmd" : "update_ops_sec",
        "graph_values" : arr.tolist(),
        "max_value" : "%d" % arr_max,
    }
    return ops_sec

def run_main():
    print "Stat Cube Redis Monitor"
    stat_cube = serial.Serial("/dev/ttyUSB0", 9600)
    redis_conn = redis.StrictRedis(host='localhost', port=6379, db=0)
    ops_sec_deque = deque([0] * 40)

    print "Monitoring Redis..."
    while True:
        info = redis_conn.info()

        _ = ops_sec_deque.popleft()
        ops_sec_deque.append(info["instantaneous_ops_per_sec"])

        basic_info = json.dumps(build_update_basic(info))
        stat_cube.write(basic_info + '\n')
        stat_cube.flush()
        time.sleep(0.3)

        advanced_info = json.dumps(build_update_advanced(info))
        stat_cube.write(advanced_info + '\n')
        stat_cube.flush()
        time.sleep(0.3)

        ops_sec_info = json.dumps(build_update_ops_sec(ops_sec_deque))
        stat_cube.write(ops_sec_info + '\n')
        stat_cube.flush()
        time.sleep(0.3)

        time.sleep(1)

    
if __name__ == '__main__':
    run_main()