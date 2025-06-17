#!/usr/bin/env python3
import os
import json

UPLOAD_DIR = "/home/welow/webserv-test/webserv/cgi-bin/storage"

print("Content-Type: application/json\r\n")

try:
    files = os.listdir(UPLOAD_DIR) # list all files in the upload directory
    files = [f for f in files if os.path.isfile(os.path.join(UPLOAD_DIR, f))] # filter out directories
    print(json.dumps({"files": files})) # return the list of files as JSON
except Exception as e:
    print(json.dumps({"error": str(e)})) # return an error message if something goes wrong
