#!/usr/bin/env python3

import signal
import os
import json

def signal_handler(signum, frame):
    os._exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
UPLOAD_DIR = os.path.join(SCRIPT_DIR, "storage")

print("Content-Type: application/json\r\n")

try:
    files = os.listdir(UPLOAD_DIR) # list all files in the upload directory
    files = [f for f in files if os.path.isfile(os.path.join(UPLOAD_DIR, f))] # filter out directories
    print(json.dumps({"files": files})) # return the list of files as JSON
except Exception as e:
    print(json.dumps({"error": str(e)})) # return an error message if something goes wrong
