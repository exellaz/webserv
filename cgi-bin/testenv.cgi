#!/usr/bin/python3

import os

print("Content-Type: text/plain\r\n")
print("\r\n")
print("these are the cgi environment variables\r\n")
for i, (key, value) in enumerate(os.environ.items()):
    print(f"{i}: {key}={value}\r\n")
