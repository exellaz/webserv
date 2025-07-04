#!/usr/bin/python3

import signal
import os

def signal_handler(signum, frame):
    os._exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

print("Content-Type: text/plain\r\n")
print("\r\n")
print("these are the cgi environment variables\r\n")
for i, (key, value) in enumerate(os.environ.items()):
    print(f"{i}: {key}={value}\r\n")
